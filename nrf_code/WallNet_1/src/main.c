#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>

#include "fp_commands.h"

#include "system_state.h"

#include "buzzer.h"
#include "wallnet_rfid.h"

LOG_MODULE_REGISTER(main_app, LOG_LEVEL_INF);
volatile wallnet_state_t sys_current_state = STATE_BOOT_CHECK;

#define RFID_URL_PREFIX "rachelchen22.github.io/#"
#define RFID_PACKET_MAX_LEN (MAX_LAST_FOUR_LEN + MAX_NAME_LEN + MAX_NAME_LEN + \
                             MAX_ENC_PAN_LEN + MAX_ENC_CVV_LEN + MAX_ENC_EXP_LEN + 6)
#define RFID_URL_MAX_LEN (sizeof(RFID_URL_PREFIX) + RFID_PACKET_MAX_LEN)

volatile bool sys_is_connected = false;
volatile bool sys_is_bonded = false;
volatile uint8_t sys_active_card_idx = 0;
volatile uint8_t sys_num_cards = 0;


card_record_t sys_card_slots[MAX_CARDS] = {0};
struct gps_telemetry_t sys_current_gps_payload = {0};
volatile bool sys_have_valid_gps = false;
volatile bool sys_is_armed = false; // For future fingerprint auth / RFID transmission state

// fingerprint queuee
K_THREAD_STACK_DEFINE(fp_workq_stack, 2048);
struct k_work_q fp_workq;
struct k_work_delayable auth_check_work;
struct k_work_delayable rfid_timeout_work;
// enrollment worker
static struct k_work_delayable fp_enroll_work;
uint16_t sys_num_fingers = 0;

static int build_rfid_packet(const card_record_t *card, char *packet_buf, size_t packet_buf_size)
{
    size_t first_name_len = strnlen(card->first_name, sizeof(card->first_name));
    size_t last_name_len = strnlen(card->last_name, sizeof(card->last_name));
    size_t last_four_len = strnlen(card->last_four, sizeof(card->last_four));

    int written = snprintk(packet_buf, packet_buf_size,
                           "%.*s|%.*s|%.*s|%.*s|%.*s|%.*s",
                           (int)last_four_len, card->last_four,
                           (int)first_name_len, card->first_name,
                           (int)last_name_len, card->last_name,
                           (int)card->enc_pan_len, card->enc_pan,
                           (int)card->enc_cvv_len, card->enc_cvv,
                           (int)card->enc_exp_len, card->enc_exp);

    if ((written < 0) || ((size_t)written >= packet_buf_size)) {
        LOG_ERR("RFID packet build failed: buffer too small.");
        return -ENOMEM;
    }

    return written;
}

static int build_rfid_url(const card_record_t *card, char *url_buf, size_t url_buf_size)
{
    char packet_buf[RFID_PACKET_MAX_LEN];
    int packet_len = build_rfid_packet(card, packet_buf, sizeof(packet_buf));

    if (packet_len < 0) {
        return packet_len;
    }

    int written = snprintk(url_buf, url_buf_size, "%s%s", RFID_URL_PREFIX, packet_buf);
    if ((written < 0) || ((size_t)written >= url_buf_size)) {
        LOG_ERR("RFID URL build failed: buffer too small.");
        return -ENOMEM;
    }

    return written;
}

// NVM Boot loader
// Called automatically by settings_load()
static int wallet_settings_set(const char *name, size_t len, settings_read_cb read_cb, void *cb_arg)
{
    const char *next;
    int rc;

    if (settings_name_steq(name, "cards", &next) && !next) {
        if ((len > sizeof(sys_card_slots)) || ((len % sizeof(card_record_t)) != 0U)) {
            LOG_ERR("Settings length mismatch for cards! Max %zu, got %zu", sizeof(sys_card_slots), len);
            return -EINVAL;
        }

        memset(sys_card_slots, 0, sizeof(sys_card_slots));

        // Pull data from Flash into our global RAM array
        rc = read_cb(cb_arg, sys_card_slots, len);
        if (rc >= 0) {
            // Boot-up math: Count how many valid cards were just loaded
            sys_num_cards = 0;
            for(int i = 0; i < MAX_CARDS; i++) {
                if (sys_card_slots[i].valid) {
                    sys_num_cards++;
                }
            }
            
            LOG_WRN("--- NVM LOAD SUCCESS ---");
            LOG_WRN("Restored %d cards from storage.", sys_num_cards);
            return 0;
        }
        return rc;
    }

    if (settings_name_steq(name, "finger_count", &next) && !next) {
        uint16_t loaded_finger_count = 0;

        if (len != sizeof(loaded_finger_count)) {
            LOG_ERR("Settings length mismatch for finger_count! Expected %zu, got %zu",
                    sizeof(loaded_finger_count), len);
            return -EINVAL;
        }

        rc = read_cb(cb_arg, &loaded_finger_count, sizeof(loaded_finger_count));
        if (rc >= 0) {
            sys_num_fingers = loaded_finger_count;
            LOG_WRN("Restored %u fingerprints from storage.", sys_num_fingers);
            return 0;
        }
        return rc;
    }

    return -ENOENT;
}


// handler fires after 10 seconds of rfid transmit state
static void rfid_timeout_handler(struct k_work *work) {
    LOG_WRN("RFID Timeout reached. Relocking.");
    
    wallnet_gps_start(); // restart gps after rfid timeout
    sys_is_armed = false;
    sys_current_state = STATE_LOCKED;
    update_eink_display();
    
    // resume fp polling
    k_work_reschedule_for_queue(&fp_workq, &auth_check_work, K_NO_WAIT);
}


static void auth_check_worker(struct k_work *work) {
    char rfid_url[RFID_URL_MAX_LEN];

    // only check auth in locked state, check back in 500ms
    if (sys_current_state != STATE_LOCKED) {
        k_work_reschedule_for_queue(&fp_workq, &auth_check_work, K_MSEC(500));
        return; 
    }

    // poll fp scanner
    if (start_and_identify()) {
        LOG_WRN("MATCH! Unlocking...");
        sys_is_armed = true; 
        sys_current_state = STATE_RFID_TRANSMITTING;
        update_eink_display(); 
        
        // start RFID timeout clk
        k_work_reschedule(&rfid_timeout_work, K_SECONDS(10));
        
        // stop fp while RFID is active
        k_work_reschedule_for_queue(&fp_workq, &auth_check_work, K_SECONDS(11));
        wallnet_gps_stop();// gps off during rfid for i2c

        if ((sys_num_cards == 0) || (sys_active_card_idx >= sys_num_cards)) {
            LOG_ERR("RFID transmit aborted: active card index is invalid.");
        } else {
            int url_len = build_rfid_url(&sys_card_slots[sys_active_card_idx],
                                         rfid_url, sizeof(rfid_url));
            
            int ret;

            uint8_t cc[4] = {
                0xE1,
                0x40,
                0x20,
                0x00
            };

            st25dv_write_bytes(0x0000, cc, 4);

            ret = wallnet_rfid_write_url(rfid_url);
            printk("length: %d\n", strlen(rfid_url));

        }


    } else { // otherwise restart in 2s
        // LOG_ERR("Bad scan detected. Resting for 2 seconds...");
        k_work_reschedule_for_queue(&fp_workq, &auth_check_work, K_SECONDS(2));
    }
}


// Additional Enrollment Worker
static void fp_enroll_worker(struct k_work *work) {
    uint16_t next_fp_id = sys_num_fingers + 1;

    LOG_WRN("Starting Add Fingerprint routine for ID: %d", next_fp_id);
        
    if (start_and_enroll(next_fp_id, 3, true, true, true, true)) {
        int err;

        LOG_WRN("New fingerprint successfully added!");
        sys_num_fingers++;
        err = wallnet_save_fingerprint_count_to_nvm();
        if (err) {
            LOG_ERR("Fingerprint enrolled, but finger count save failed.");
        }
    } else {
        LOG_ERR("Failed to add fingerprint.");
    }
    
    sys_current_state = STATE_LOCKED;
    update_eink_display();
    
    // Kick the background scanner back on -- 3s wait so dont match immediately and shoot RFID off
    k_work_reschedule_for_queue(&fp_workq, &auth_check_work, K_SECONDS(3));
}

// trigger to start fp enroll
void wallnet_enroll_trigger(void) {
    printk("Enrollment triggered! Starting in 1 second...");
    
    sys_current_state = STATE_ENROLLING;
    update_eink_display();
    
    // Kill fp scanner
    k_work_cancel_delayable(&auth_check_work);
    
    // Schedule the enrollment worker for 1 second in the future
    k_work_reschedule_for_queue(&fp_workq, &fp_enroll_work, K_SECONDS(1));
}

// block off memory for cards in flash so we can write them to NVS/NVM w/ settings_save_one()
static struct settings_handler wallet_conf = {
    .name = "wallet",
    .h_set = wallet_settings_set
};

int wallnet_save_cards_to_nvm(void) {
    int err;

    if (!IS_ENABLED(CONFIG_SETTINGS)) {
        LOG_ERR("Cannot save cards: settings subsystem is disabled.");
        return -ENOTSUP;
    }

    if (sys_num_cards == 0) {
        err = settings_delete("wallet/cards"); // if sending 0 cards, delete wallet? like a user wants to overwrite all cards
    } else {
        err = settings_save_one("wallet/cards", sys_card_slots, sys_num_cards * sizeof(card_record_t));
    }

    if (err) {
        LOG_ERR("Failed to save wallet cards to NVM (%d)", err);
        return err;
    }

    LOG_WRN("Wallet cards saved to NVM.");
    return 0;
}

int wallnet_save_fingerprint_count_to_nvm(void) {
    int err;

    if (!IS_ENABLED(CONFIG_SETTINGS)) {
        LOG_ERR("Cannot save fingerprint count: settings subsystem is disabled.");
        return -ENOTSUP;
    }

    err = settings_save_one("wallet/finger_count", &sys_num_fingers, sizeof(sys_num_fingers));
    if (err) {
        LOG_ERR("Failed to save finger_count to NVM (%d)", err);
        return err;
    }

    LOG_WRN("Fingerprint count saved to NVM: %u", sys_num_fingers);
    return 0;
}

// trigger to start fp auth
void wallnet_auth_trigger(void) {
    k_work_reschedule_for_queue(&fp_workq, &auth_check_work, K_NO_WAIT);
}


int main(void)
{
    int err;
    printk("WallNet Booting...");

    wallnet_ble_init();
    //Note for Austin: to work without gps, comment out wallnet_gps_init() and wallnet_gps.c : wallnet_gps_start()
    wallnet_gps_init(); // takes care of gps_conf before settings_load()
    wallnet_ui_init();

    buzzer_init();

    wallnet_rfid_init();

    
    // NVM storage for cards/gps
    if (IS_ENABLED(CONFIG_SETTINGS)) {
        err = settings_register(&wallet_conf);
        if (err) {
            LOG_ERR("Failed to register settings handler (err %d)", err);
        }

        // triggers ALL registered settings_handlers
        err = settings_load();
        if (err) {
            LOG_ERR("Settings load failed (err %d)", err);
        }
    }

    // fingerprint work threads
    k_work_queue_start(&fp_workq, fp_workq_stack, K_THREAD_STACK_SIZEOF(fp_workq_stack), 7, NULL);
    k_work_init_delayable(&auth_check_work, auth_check_worker);
    k_work_init_delayable(&rfid_timeout_work, rfid_timeout_handler);
    
    k_work_init_delayable(&fp_enroll_work, fp_enroll_worker);


    if (sys_num_cards == 0) {
        sys_current_state = STATE_INIT_WAIT_ENROLL;
        printk("Entering First-Time Setup.");
    } else { // if we have cards, we must have fingerprint so go right into locked
        sys_current_state = STATE_LOCKED;
        printk("Existing data found. Vault Locked.");
        wallnet_ble_adv_start(); // Automatically start searching for phone

        fp_init();
        k_work_reschedule_for_queue(&fp_workq, &auth_check_work, K_NO_WAIT);
    }

    printk("Fingerprint/Bluetooth setup, moving on to event interrupts.");
    
    update_eink_display();

    // Main Loop
    while (1) {
        k_sleep(K_FOREVER);
    }

    return 0;
}
