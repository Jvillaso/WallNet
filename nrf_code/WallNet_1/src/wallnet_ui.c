#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/input/input.h>

#include "eink.h"
#include "draw.h"

#include "system_state.h"
#include "fp_commands.h"

LOG_MODULE_REGISTER(wallnet_ui, LOG_LEVEL_INF);

static int64_t press_start_time = 0;
static uint8_t tap_count = 0;

static struct k_work_delayable tap_eval_work;
static struct k_work_delayable factory_reset_work;

void update_eink_display(void) {
    switch (sys_current_state) {
        case STATE_INIT_WAIT_ENROLL:
            LOG_WRN("\n[E-INK] Setup 1/2: Press button to enroll Fingerprint.");
            displayErr("Press button to enroll Fingerprint");
            break;
        case STATE_ENROLLING:
            LOG_WRN("\n[E-INK] Enrolling Fingerprint...");
            LOG_WRN("\n[E-INK] Rapidly place finger on sensor three times...");
            displayErr("Place finger on sensor 3x");

            // todo add something here to show enrollment success/failure
            // maybe a section of screen that displays status message for a few seconds
            break;

        case STATE_INIT_WAIT_PAIRING:
            LOG_WRN("\n[E-INK] Setup 2/2: Press button to begin Bluetooth pairing.");
            displayErr("Press button to begin Bluetooth pairing");
            break;
        case STATE_PAIRING_ADVERTISING:
            LOG_WRN("\n[E-INK] Pairing Mode: Open App to Connect...");
            displayErr("pen App to Connect");
            break;
        case STATE_BLE_SYNCING:
            LOG_WRN("\n[E-INK] Syncing Cards with Phone... Please Wait.");
            displayErr("Syncing Cards with Phone Please Wait");
            break;
        case STATE_LOCKED:
            if (sys_num_cards == 0) {
                LOG_WRN("\n[E-INK] No Cards Saved.");
                displayErr("No Cards Saved");
            } else {
                if (sys_active_card_idx >= sys_num_cards) sys_active_card_idx = 0;
                
                LOG_WRN("\n[E-INK] Displaying Card %d/%d: %s %s (...%s)", 
                        sys_active_card_idx + 1, sys_num_cards, 
                        sys_card_slots[sys_active_card_idx].first_name,
                        sys_card_slots[sys_active_card_idx].last_name,
                        sys_card_slots[sys_active_card_idx].last_four);

                displayCard(sys_card_slots[sys_active_card_idx]);
                
            }
            break;
        case STATE_RFID_TRANSMITTING:
            LOG_WRN("\n[E-INK] TRANSMITTING: %s %s - (...%s)", 
                    sys_card_slots[sys_active_card_idx].first_name,
                    sys_card_slots[sys_active_card_idx].last_name,
                    sys_card_slots[sys_active_card_idx].last_four);
            displayErr("Transmitting Card Data");
            break;
        case STATE_RESET:
            LOG_WRN("\n[E-INK] !!! FACTORY RESET INITIATED !!!");
            LOG_WRN("\n[E-INK] All data wiped. Rebooting...");
            // todo: clear screen?
            // maybe wait for them to cancel and say "Resetting WallNet... Press Button to Cancel" otherwise it clears screen and reboots

            displayErr("Resetting");
            break;
        default:
            break;
    }
}

// Fires 400ms after the LAST short tap
static void tap_eval_handler(struct k_work *work) {
    
    switch (sys_current_state) {
        case STATE_INIT_WAIT_ENROLL:
            // Pretend the user scanned their finger successfully
            // LOG_WRN("SIMULATED FINGERPRINT SUCCESS.");
            // printk("Starting enrollment process...");
                        
            sys_current_state = STATE_ENROLLING;
            update_eink_display();
            fp_init();

            // id=1
            if(start_and_enroll(1, 3, true, true, true, true)) {
                sys_num_fingers = 1;
                if (wallnet_save_fingerprint_count_to_nvm()) {
                    LOG_ERR("Initial fingerprint enrolled, but finger count save failed.");
                }
                // LOG_WRN("[E-INK] Enrollment successful.");
                sys_current_state = STATE_INIT_WAIT_PAIRING;
                update_eink_display();
            } else {
                LOG_ERR("Enrollment failed. Please try again.");
                sys_current_state = STATE_INIT_WAIT_ENROLL;
                update_eink_display();
            }
            break;

        case STATE_INIT_WAIT_PAIRING:
            // Start Bluetooth and wait for phone
            sys_current_state = STATE_PAIRING_ADVERTISING;
            update_eink_display();
            wallnet_ble_adv_start();
            break;

        case STATE_LOCKED:
        
            if (sys_num_cards == 0) {
                printk("No cards in wallet. Ignoring taps."); // dont update eink for this: still says "No Cards Saved"
            } else {
                sys_active_card_idx = (sys_active_card_idx + tap_count) % sys_num_cards;
                printk("Registered %d taps. Active Card Index is now %d.", tap_count, sys_active_card_idx);
                update_eink_display();
            }
            break;

        case STATE_BLE_SYNCING:
            printk("Device is syncing over BLE. Ignoring button tap."); // don't update eink 
            break;

        default:
            break;
    }
    
    tap_count = 0;
}

// Fires if the button is held for 10s
static void factory_reset_handler(struct k_work *work) {

    // Reject if in middle of saving cards to NVM
    // started using this for boot sequ, have to update
    if (sys_current_state == STATE_BLE_SYNCING) {
        LOG_ERR("Reset blocked: System is currently locking NVM.");
        return;
    }

    sys_current_state = STATE_RESET;
    update_eink_display();

    // Wipe WallNet
    memset(sys_card_slots, 0, sizeof(sys_card_slots));
    sys_num_cards = 0;
    sys_active_card_idx = 0;
    sys_is_bonded = false;
    sys_num_fingers = 0;

    // Wipe WallNet NVM
    settings_delete("wallet/cards");
    settings_delete("wallet/finger_count");
    settings_delete("gps/last_fix");
    
    // Wipe Zephyr security keys (bonds from ble)
    bt_unpair(BT_ID_DEFAULT, BT_ADDR_LE_ANY);

    // Wipe the fingerprint scanner
    empty();
    
    update_eink_display();
    
    printk("Wallet wiped. Rebooting system in 2 seconds to clear state...");
    
    // I was thinking wait for the eink to redraw
    k_sleep(K_SECONDS(2));
    // then hardware reboot
    sys_reboot(SYS_REBOOT_WARM);
}

//  HARDWARE ISR way easier zephyr version does all the debounce and handling apart of work systme
static void input_cb(struct input_event *evt, void *user_data) {
    if (evt->code != INPUT_KEY_0) {
        return;
    }

    //TODO: change below to 0 for active low button, this is just for the dev board
    if (evt->value == 1) { 
        // Falling EDGE (PRESSED)
        press_start_time = k_uptime_get();
        
        // Start the 10s doomsday
        k_work_reschedule(&factory_reset_work, K_SECONDS(10));
        
    } else { 
        // Rising EDGE (RELEASED)
        int64_t duration = k_uptime_get() - press_start_time;
        
        // cancel doomsdasy if button released before 10s
        k_work_cancel_delayable(&factory_reset_work);

        if (duration < 1000) {
            // SHORT PRESS (<1s)
            tap_count++;
            // wait 400ms for another tap
            k_work_reschedule(&tap_eval_work, K_MSEC(400));
            
        } else if (duration >= 1000 && duration < 10000) {
            // HOLD (1s - 10s)

            tap_count = 0; // any amt of taps, then a hold counts as a hold
            k_work_cancel_delayable(&tap_eval_work);
            
            if (sys_current_state == STATE_LOCKED) {
                // Trigger the enrollment routine
                printk("Hold detected (%lld ms). Entering Fingerprint Enrollment.", duration);
                printk("Entering Additional Enrollment Mode.");
                sys_current_state = STATE_ENROLLING;
                update_eink_display();
                
                wallnet_enroll_trigger();
            } else {
                printk("Hold detected but system is not in LOCKED state. Ignoring.");
            }

        } else if (duration >= 10000) {
            // fallback, doomsday should auto-trigger at 10s before this
            k_work_reschedule(&factory_reset_work, K_NO_WAIT);
        }
    }
}


INPUT_CALLBACK_DEFINE(NULL, input_cb, NULL);

void wallnet_ui_init(void) {
    k_work_init_delayable(&tap_eval_work, tap_eval_handler);
    k_work_init_delayable(&factory_reset_work, factory_reset_handler);
    
    LOG_WRN("UI Subsystem Initialized. Input callbacks registered.");
}
