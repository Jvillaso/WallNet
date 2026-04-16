#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/settings/settings.h>
#include <zephyr/logging/log.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <zephyr/sys/util.h> // Required for MIN() macro (parsing encrypted cards)

#include "system_state.h"

LOG_MODULE_REGISTER(wallnet_ble, LOG_LEVEL_INF);

// UUIDs kind of like phone numbers, defines who or what we are talking to
#define BT_UUID_GPS_SERVICE_VAL \
    BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef0)
#define BT_UUID_GPS_CHAR_VAL \
    BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef1)
#define BT_UUID_WALLET_PACKET_CHAR_VAL \
    BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef3)
#define BT_UUID_TEST_CHAR_VAL \
    BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef4)

#define BT_UUID_GPS_SERVICE         BT_UUID_DECLARE_128(BT_UUID_GPS_SERVICE_VAL)
#define BT_UUID_GPS_CHAR            BT_UUID_DECLARE_128(BT_UUID_GPS_CHAR_VAL)
#define BT_UUID_WALLET_PACKET_CHAR  BT_UUID_DECLARE_128(BT_UUID_WALLET_PACKET_CHAR_VAL)
#define BT_UUID_TEST_CHAR           BT_UUID_DECLARE_128(BT_UUID_TEST_CHAR_VAL)

// ble Advertising data - what the phone sees when scanning for WallNet
static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_GPS_SERVICE_VAL),
};

static const struct bt_data sd[] = {
    BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};

static struct k_work_delayable wallet_save_work;
static struct k_work adv_restart_work;
// static bool is_receiving_wallet = false;


static void append_to_field(uint8_t *dest_array, uint8_t *current_len, 
                           uint8_t *new_data, uint8_t new_len, 
                           uint8_t max_size) 
{
    // how much room is actually left in struct
    uint8_t space_left = max_size - (*current_len);
    
    // how many bytes we can safely copy
    uint8_t to_copy = (new_len < space_left) ? new_len : space_left;

    if (to_copy > 0) {
        // copy data starting at the end of what we already have
        memcpy(&dest_array[*current_len], new_data, to_copy);
        
        // update the length tracker 
        *current_len += to_copy;
    } else {
        LOG_ERR("Field overflow - Cannot append %d bytes.", new_len);
    }
}

static uint8_t rx_buffer[512]; // not rly sure how big this needs ot be
static uint16_t rx_idx = 0;
static uint8_t shadow_card_idx = 0;
static card_record_t shadow_card_slots[MAX_CARDS];

// finds the end of a card by jumping over packet lengths
static uint16_t get_card_chunk_length(uint8_t *buf, uint16_t current_len) {
    uint16_t i = 0;
    while (i < current_len) {
        if (i + 1 >= current_len) return 0; // Not enough bytes for flag+len

        uint8_t flag = buf[i];
        uint8_t length = buf[i + 1];
        uint16_t packet_size = 2 + length + 1; // Flag + Length + Data + Checksum

        if (i + packet_size > current_len) return 0; // Wait for more BLE chunks

        if (flag == 0x02) {
            return i + packet_size; // find stop byte for total len -- are we storing this in the start_byte or something arleady? 
        }
        i += packet_size;
    }
    return 0; // No complete card in the buffer yet
}

static void parse_card(uint8_t *buffer, uint16_t len) {
    uint16_t i = 0;
    card_record_t *current_card = &shadow_card_slots[shadow_card_idx];

    while (i < len) {
        uint8_t flag = buffer[i]; // first byte
        uint8_t length = buffer[i + 1]; // second byte
        uint16_t packet_size = 2 + length + 1;  // size of packet
        
        uint8_t *data_ptr = &buffer[i + 2]; // ptr starts at data begin
        uint8_t expected_checksum = buffer[i + 2 + length]; // last byte checksum

        uint8_t calculated_chk = flag ^ length;
        for (uint8_t d = 0; d < length; d++) {
            calculated_chk ^= data_ptr[d]; // calculate checksum w/ xor
        }

        if (calculated_chk != expected_checksum) {
            LOG_ERR("Checksum failed for flag: 0x%02X", flag);
        } else {
            switch (flag) {
                case 0x01: // Start Byte
                    memset(current_card, 0, sizeof(card_record_t));
                    break;
                    
                case 0x03: // Last 4
                    memcpy(current_card->last_four, data_ptr, MIN(length, MAX_LAST_FOUR_LEN - 1));
                    current_card->last_four[MIN(length, MAX_LAST_FOUR_LEN - 1)] = '\0';
                    break;
                    
                case 0x04: // First Name
                    memcpy(current_card->first_name, data_ptr, MIN(length, MAX_NAME_LEN - 1));
                    current_card->first_name[MIN(length, MAX_NAME_LEN - 1)] = '\0';
                    break;
                    
                case 0x05: // Last Name
                    memcpy(current_card->last_name, data_ptr, MIN(length, MAX_NAME_LEN - 1));
                    current_card->last_name[MIN(length, MAX_NAME_LEN - 1)] = '\0';
                    break;
                    
                case 0x06: case 0x07: // Enc card number
                    append_to_field(current_card->enc_pan, &current_card->enc_pan_len, 
                                    data_ptr, length, MAX_ENC_PAN_LEN);
                    break;
                    
                case 0x08: case 0x09: // Enc CVV
                    append_to_field(current_card->enc_cvv, &current_card->enc_cvv_len, 
                                    data_ptr, length, MAX_ENC_CVV_LEN);
                    break;
                    
                case 0x0A: case 0x0B: // Enc EXP
                    append_to_field(current_card->enc_exp, &current_card->enc_exp_len, 
                                    data_ptr, length, MAX_ENC_EXP_LEN);
                    break;

                case 0x02: // Stop Byte
                    current_card->valid = true;
                    LOG_WRN("Parsed Card %d: %s %s (...%s)", shadow_card_idx + 1, 
                            current_card->first_name, current_card->last_name, current_card->last_four);
                    
                    // Setup for the next card to arrive
                    shadow_card_idx++;
                    break;
            }
        }
        i += packet_size; 
    }
}


static ssize_t write_wallet_packet(struct bt_conn *conn,
                                   const struct bt_gatt_attr *attr,
                                   const void *buf, uint16_t len,
                                   uint16_t offset, uint8_t flags)
{
    const uint8_t *byte_buf = (const uint8_t *)buf;
    
    k_work_cancel_delayable(&auth_check_work); // stop scanning fp while writing cards

    if (sys_current_state != STATE_BLE_SYNCING) {
        LOG_WRN("New Wallet Packet. Entering BLE Sync State...");
        sys_current_state = STATE_BLE_SYNCING; 
        rx_idx = 0; 
        shadow_card_idx = 0;
    }

    // incoming bytes
    for (uint16_t k = 0; k < len; k++) {
        if (rx_idx < sizeof(rx_buffer)) rx_buffer[rx_idx++] = byte_buf[k];
    }

    uint16_t card_len = get_card_chunk_length(rx_buffer, rx_idx);
    
    while (card_len > 0) {
        // [Start...Stop] chunk
        parse_card(rx_buffer, card_len);

        // Shift buffer to remove the parsed card and keep listening
        uint16_t remaining = rx_idx - card_len;
        if (remaining > 0) {
            memmove(rx_buffer, &rx_buffer[card_len], remaining);
        }
        rx_idx = remaining;

        // If we hit 16 cards, stop waiting and save immediately
        if (shadow_card_idx >= MAX_CARDS) {
            k_work_cancel_delayable(&wallet_save_work);
            k_work_schedule(&wallet_save_work, K_NO_WAIT);
            return len;
        }

        // Check if there is ANOTHER complete card waiting in the shifted buffer
        card_len = get_card_chunk_length(rx_buffer, rx_idx);
    }

    // 2-second timeout to see if we get more chunks (cards)
    k_work_reschedule(&wallet_save_work, K_MSEC(2000));

    return len;
}

static void wallet_save_timeout_handler(struct k_work *work) {
    uint8_t valid_count = 0;

    for (int i = 0; i < MAX_CARDS; i++) {
        if (shadow_card_slots[i].valid) {
            valid_count++;
        }
    }

    if (valid_count > 0) {
        LOG_WRN("Validation passed. Promoting shadow cards to NVM.");
        memcpy(sys_card_slots, shadow_card_slots, sizeof(sys_card_slots));
        sys_num_cards = valid_count;
        sys_active_card_idx = 0; 
        
        LOG_WRN("NVM Save Success: System holds %d cards.", valid_count);
        // NVM Save function call goes here...
    } else {
        LOG_ERR("BLE Sync Timeout: No valid cards received.");
    }
    
    sys_current_state = STATE_LOCKED;
    LOG_WRN("BLE Sync Complete. System Locked.");

    // card transmission/saving done: save cards, start fp polling again
    k_work_reschedule_for_queue(&fp_workq, &auth_check_work, K_NO_WAIT);
    update_eink_display();
}


static ssize_t write_test(struct bt_conn *conn,
                                   const struct bt_gatt_attr *attr,
                                   const void *buf, uint16_t len,
                                   uint16_t offset, uint8_t flags) 
{
    printk("Received write to wallet packet characteristic. Data length: %d\n", len);   
    
    const u_int8_t *char_buf = (const u_int8_t *)buf;
    for (uint16_t i = 0; i < len; i++) {
        u_int8_t c = char_buf[i];
        printk("0x%02X ('%c')", c, (c >= 32 && c <= 126) ? c : '?');
    }
    printk("\n");

    return len;
}

// fires when phone requests to read GPS data
static ssize_t read_gps_data(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                             void *buf, uint16_t len, uint16_t offset)
{
    const void *value = attr->user_data;
    return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
                             sizeof(struct gps_telemetry_t));
}

// Fires when phone subscribes/unsubscribes to GPS notifications
static void gps_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value) {
    bool notify_enabled = (value == BT_GATT_CCC_NOTIFY);
    LOG_WRN("GPS notifications %s", notify_enabled ? "enabled" : "disabled");
}

// tells Zephyr (nrf) how to build ble profile of WallNet (what phone sees)
BT_GATT_SERVICE_DEFINE(wallnet_svc,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_GPS_SERVICE),

    // GPS characteristic
    BT_GATT_CHARACTERISTIC(BT_UUID_GPS_CHAR,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_READ,
                           read_gps_data, NULL, &sys_current_gps_payload),
    // phone "subscribes" to this GPS characteristic to get notified when we have a new GPS payload to send
    BT_GATT_CCC(gps_ccc_cfg_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

    // Wallet Packet characteristic (how we send cards to WallNet)
    BT_GATT_CHARACTERISTIC(BT_UUID_WALLET_PACKET_CHAR,
                           BT_GATT_CHRC_WRITE,
                           BT_GATT_PERM_WRITE, //BT_GATT_PERM_WRITE_ENCRYPT, 
                           NULL, write_wallet_packet, NULL),

    // Test characteristic for dev/debug
    BT_GATT_CHARACTERISTIC(BT_UUID_TEST_CHAR,
                           BT_GATT_CHRC_WRITE,
                           BT_GATT_PERM_WRITE,
                           NULL, write_test, NULL),
);

static void adv_restart_handler(struct k_work *work) {
    int err = bt_le_adv_start(BT_LE_ADV_CONN_FAST_1, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
    if (err) {
        LOG_ERR("Failed to restart advertising (err %d)", err);
    } else {
        LOG_INF("Advertising restarted. Waiting for paired phone...");
    }
}

static void connected(struct bt_conn *conn, uint8_t err) {
    if (err) {
        LOG_ERR("Connection failed (err 0x%02x)", err);
    } else {
        sys_is_connected = true;
        LOG_WRN("Connected to Phone");

        if (sys_current_state == STATE_PAIRING_ADVERTISING) {
            // waiting for pairing
            sys_current_state = STATE_LOCKED;
            if (sys_num_cards > 0) { // if have cards, start looking for fp
                k_work_reschedule_for_queue(&fp_workq, &auth_check_work, K_NO_WAIT);
            }

            update_eink_display(); 
        }

        
        wallnet_gps_start();
    }
}

static void disconnected(struct bt_conn *conn, uint8_t reason) {
    sys_is_connected = false;
    LOG_INF("Disconnected from Phone (reason 0x%02x).", reason);
    
    // if have valid gps and we disconnect, save the last known to nrf NVM
    if (sys_have_valid_gps) {
        int err = settings_save_one("gps/last_fix", &sys_current_gps_payload, sizeof(sys_current_gps_payload));
        if (err) LOG_ERR("Failed to anchor GPS to NVM (%d)", err);
        else LOG_INF("Anchored Last Known GPS to NVM.");
    }

    wallnet_gps_stop();
    k_work_submit(&adv_restart_work);
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected = connected,
    .disconnected = disconnected,
};

static void auth_pairing_complete(struct bt_conn *conn, bool bonded) {
    LOG_WRN("Pairing Complete. Bonded: %s", bonded ? "Yes" : "No");
    if (bonded) {
        sys_is_bonded = true;
        if (sys_current_state == STATE_PAIRING_ADVERTISING || sys_current_state == STATE_BLE_SYNCING) {
            sys_current_state = STATE_LOCKED;
        }
    }
    update_eink_display();
}

static struct bt_conn_auth_info_cb conn_auth_info_callbacks = {
    .pairing_complete = auth_pairing_complete,
};

void wallnet_ble_notify_gps(void) {
    if (sys_is_connected && sys_have_valid_gps) {
        bt_gatt_notify(NULL, &wallnet_svc.attrs[2], 
                       &sys_current_gps_payload, sizeof(sys_current_gps_payload));
        LOG_WRN("Pushed GPS Notification to Phone.");
    }
}

void wallnet_ble_init(void) {
    int err = bt_enable(NULL);
    if (err) {
        LOG_ERR("Bluetooth init failed (err %d)", err);
        return;
    }
    
    err = bt_conn_auth_info_cb_register(&conn_auth_info_callbacks);
    if (err) LOG_ERR("Failed to register auth info callbacks (%d)", err);
    
    k_work_init_delayable(&wallet_save_work, wallet_save_timeout_handler);
    k_work_init(&adv_restart_work, adv_restart_handler);
    
    LOG_WRN("BLE Stack Initialized. Waiting for NVM to load MAC address...");
}

void wallnet_ble_adv_start(void) {
    // TODO: update advertising interval to 100ms for design req: BT_LE_ADV_CONN_FAST_1 is 30-60ms
    int err = bt_le_adv_start(BT_LE_ADV_CONN_FAST_1, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
    if (err) {
        LOG_ERR("Advertising failed to start (err %d)", err);
        return;
    }
    
    LOG_WRN("BLE Advertising successfully started.");
}