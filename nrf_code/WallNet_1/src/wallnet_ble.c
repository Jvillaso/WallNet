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
static bool is_receiving_wallet = false;

// buffer for mit app inventor chunks
static char rx_buffer[128];
static uint16_t rx_idx = 0;
static uint8_t shadow_card_idx = 0;
static card_record_t shadow_card_slots[MAX_CARDS];


// update after encryption implemented
static void parse_and_store_card(char *line) {

    char *save_ptr;

    // strtok_r chops into pieces saves ending pt
    // strtok is evil no threads
    char *name = strtok_r(line, "|", &save_ptr);
    char *num = strtok_r(NULL, "|", &save_ptr);
    char *exp = strtok_r(NULL, "|", &save_ptr);
    char *cvv = strtok_r(NULL, "|", &save_ptr);

    // check we found everything and don't overeat
    if (name && num && exp && cvv && shadow_card_idx < MAX_CARDS) {
        
        // wipe specific slot before writing to it just in case
        memset(&shadow_card_slots[shadow_card_idx], 0, sizeof(card_record_t));
        
        // copy data right in
        shadow_card_slots[shadow_card_idx].valid = true;
        strncpy(shadow_card_slots[shadow_card_idx].cardholder, name, MAX_CARDHOLDER_LEN - 1);
        strncpy(shadow_card_slots[shadow_card_idx].cardnumber, num, MAX_CARDNUMBER_LEN - 1);
        strncpy(shadow_card_slots[shadow_card_idx].exp, exp, MAX_EXP_LEN - 1);
        strncpy(shadow_card_slots[shadow_card_idx].cvv, cvv, MAX_CVV_LEN - 1);
        
        LOG_WRN("Parsed slot %d: %s", shadow_card_idx, shadow_card_slots[shadow_card_idx].cardholder);
        shadow_card_idx++;
        
    } else {
        LOG_ERR("Malformed card data ignored.");
    }
}

// happens after 2s of no BLE wallet writes, triggers validation/save to NVM
static void wallet_save_timeout_handler(struct k_work *work) {
    is_receiving_wallet = false;
    uint8_t valid_count = 0;

    for (int i = 0; i < MAX_CARDS; i++) {
        if (shadow_card_slots[i].valid) valid_count++;
    }

    if (valid_count == 0) {
        LOG_WRN("Sync failed or empty. Retaining old cards.");
    } else {
        LOG_WRN("Validation passed. Promoting shadow cards to Live Array.");
        memcpy(sys_card_slots, shadow_card_slots, sizeof(sys_card_slots));
        sys_num_cards = valid_count;
        sys_active_card_idx = 0;

        int err = settings_save_one("wallet/cards", sys_card_slots, sizeof(sys_card_slots));
        if (err) LOG_ERR("NVM Save Failed (%d)", err);
        else LOG_WRN("NVM Save Success: System holds %d cards.", sys_num_cards);
        
    }

    // Clear shadow/rx buffer to be safe
    memset(shadow_card_slots, 0, sizeof(shadow_card_slots));
    memset(rx_buffer, 0, sizeof(rx_buffer));
    rx_idx = 0;
    shadow_card_idx = 0;

    // Release the system lock (locks button during wallet writes)
    sys_current_state = STATE_LOCKED;
    LOG_WRN("BLE Sync Complete. System Locked.");

    update_eink_display();

}

static ssize_t write_wallet_packet(struct bt_conn *conn,
                                   const struct bt_gatt_attr *attr,
                                   const void *buf, uint16_t len,
                                   uint16_t offset, uint8_t flags)
{
    ARG_UNUSED(conn);
    ARG_UNUSED(attr);

    if (len == 0) return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    if (offset != 0U) return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);

    // Only allow wallet write if LOCKED or SYNCING (write process already started, waiting for more chucnks)
    if (sys_current_state != STATE_LOCKED && sys_current_state != STATE_BLE_SYNCING) {
        LOG_ERR("BLE write rejected. System is not in LOCKED state.");
        return BT_GATT_ERR(BT_ATT_ERR_WRITE_NOT_PERMITTED); // Tell google phone to fuck off
    }

    // First part of packet, clear everything and get ready for wallet data
    if (!is_receiving_wallet) {
        LOG_WRN("New wallet transmission detected. Entering BLE Sync State...");
        memset(shadow_card_slots, 0, sizeof(shadow_card_slots));
        shadow_card_idx = 0;
        rx_idx = 0;
        is_receiving_wallet = true;
        
        // Lock out physical inputs globally
        sys_current_state = STATE_BLE_SYNCING; 
    }

    // Reschedule the commit timer every time we get a chunk
    k_work_reschedule(&wallet_save_work, K_MSEC(2000));

    // Process incoming chunks byte by byte
    const char *char_buf = (const char *)buf;
        for (uint16_t i = 0; i < len; i++) {
            char c = char_buf[i];
            
            //dbg, print every byte
            // printk("Byte RX: 0x%02x ('%c')\n", c, (c >= 32 && c <= 126) ? c : '?');

            // terminating card character
            if (c == ';') {
                rx_buffer[rx_idx] = '\0';
                parse_and_store_card(rx_buffer);
                rx_idx = 0; 
            } 
            // ignore newlines, nulls, carriage returns (weird google phone thing) and prevent buffer overflow
            else if (c != '\r' && c != '\n' && c != '\0' && rx_idx < sizeof(rx_buffer) - 1) {
                rx_buffer[rx_idx++] = c;
            }
        }

    return len;
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
            k_work_reschedule_for_queue(&fp_workq, &auth_check_work, K_NO_WAIT);

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