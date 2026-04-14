#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>

#include "fp_commands.h"

#include "system_state.h"

LOG_MODULE_REGISTER(main_app, LOG_LEVEL_INF);
volatile wallnet_state_t sys_current_state = STATE_BOOT_CHECK;

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
static struct k_work fp_enroll_work;
uint16_t sys_next_fp_id = 2; 

// NVM Boot loader
// Called automatically by settings_load()
static int wallet_settings_set(const char *name, size_t len, settings_read_cb read_cb, void *cb_arg)
{
    const char *next;
    int rc;

    if (settings_name_steq(name, "cards", &next) && !next) {
        if (len != sizeof(sys_card_slots)) {
            LOG_ERR("Settings length mismatch for cards! Expected %zu, got %zu", sizeof(sys_card_slots), len);
            return -EINVAL;
        }

        // Pull data from Flash into our global RAM array
        rc = read_cb(cb_arg, sys_card_slots, sizeof(sys_card_slots));
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

        // rfid write link here


    } else { // otherwise restart in 3s
        LOG_ERR("Bad scan detected. Resting for 3 seconds...");
        k_work_reschedule_for_queue(&fp_workq, &auth_check_work, K_SECONDS(3));
    }
}


// Additional Enrollment Worker
static void fp_enroll_worker(struct k_work *work) {
    LOG_WRN("Starting Add Fingerprint routine for ID: %d", sys_next_fp_id);
    
    // Pause the normal 3-second polling so it doesn't fight us for the UART
    k_work_cancel_delayable(&auth_check_work);
        
    if (start_and_enroll(sys_next_fp_id, 3, true, true, true, true)) {
        LOG_WRN("New fingerprint successfully added!");
        sys_next_fp_id++; // Increment so the next finger gets a new ID
    } else {
        LOG_ERR("Failed to add fingerprint.");
    }
    
    // Put the system back to normal
    sys_current_state = STATE_LOCKED;
    update_eink_display();
    
    // Kick the background scanner back on
    k_work_reschedule_for_queue(&fp_workq, &auth_check_work, K_NO_WAIT);
}


// block off memory for cards in flash so we can write them to NVS/NVM w/ settings_save_one()
static struct settings_handler wallet_conf = {
    .name = "wallet",
    .h_set = wallet_settings_set
};

// trigger to start fp auth
void wallnet_auth_trigger(void) {
    k_work_reschedule_for_queue(&fp_workq, &auth_check_work, K_NO_WAIT);
}

// trigger to start fp enroll
void wallnet_enroll_trigger(void) {
    k_work_submit_to_queue(&fp_workq, &fp_enroll_work);
}


int main(void)
{
    int err;
    LOG_WRN("WallNet Booting...");

    wallnet_ble_init();
    wallnet_gps_init(); // takes care of gps_conf before settings_load()
    wallnet_ui_init();


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
    k_work_init(&fp_enroll_work, fp_enroll_worker);


    if (sys_num_cards == 0) {
        sys_current_state = STATE_INIT_WAIT_ENROLL;
        LOG_WRN("Entering First-Time Setup.");
    } else { // if we have cards, we must have fingerprint so go right into locked
        sys_current_state = STATE_LOCKED;
        LOG_WRN("Existing data found. Vault Locked.");
        wallnet_ble_adv_start(); // Automatically start searching for phone

        fp_init();
        k_work_reschedule_for_queue(&fp_workq, &auth_check_work, K_NO_WAIT);
    }

    LOG_WRN("Fingerprint/Bluetooth setup, moving on to event interrupts.");
    
    update_eink_display();

    // Main Loop
    while (1) {
        k_sleep(K_FOREVER);
    }

    return 0;
}
