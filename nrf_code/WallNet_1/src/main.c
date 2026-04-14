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

// block off memory for cards in flash so we can write them to NVS/NVM w/ settings_save_one()
static struct settings_handler wallet_conf = {
    .name = "wallet",
    .h_set = wallet_settings_set
};


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

    // wallnet_auth_init();


    if (sys_num_cards == 0) {
        sys_current_state = STATE_INIT_WAIT_ENROLL;
        LOG_WRN("Entering First-Time Setup.");
    } else { // if we have cards, we must have fingerprint so go right into locked
        sys_current_state = STATE_LOCKED;
        LOG_WRN("Existing data found. Vault Locked.");
        wallnet_ble_adv_start(); // Automatically start searching for phone
    }

    LOG_WRN("Fingerprint/Bluetooth setup, moving on to event interrupts.");

    update_eink_display();

    // Main Loop
    while (1) {
        // k_sleep(K_FOREVER);

        switch (sys_current_state) {
            case STATE_LOCKED:
                if (start_and_identify()) {
                    LOG_WRN("[E-INK] Identification successful.");
                    sys_is_armed = true; 
                    sys_current_state = STATE_RFID_TRANSMITTING;
                    update_eink_display();
                } else {
                    k_msleep(3000);
                }
                break;
            case STATE_RFID_TRANSMITTING:
                // After successful auth, stay in this state for 10s then return to locked

                // write the rfid link thingy/stuff here, then go to sleep

                k_msleep(10000);
                sys_is_armed = false;
                sys_current_state = STATE_LOCKED;
                update_eink_display();
                break;

            default:
                k_msleep(1000);
                break;
        }

    }

    return 0;
}
