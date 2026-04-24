#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define MAX_CARDS 16

// Main WallNet State Machine
typedef enum {
    STATE_BOOT_CHECK,            // Check for existing fingerprint on boot
    STATE_INIT_WAIT_ENROLL,      // First-time setup gate; waiting for user to start scanner w/ button press
    STATE_ENROLLING,             // Fingerprint enrollment process active; wait for success or timeout
    STATE_INIT_WAIT_PAIRING,     // Second setup gate; waiting for user to press button to start BLE advertising/paring processs
    STATE_PAIRING_ADVERTISING,   // BLE radio active; wait for BT connection and pairing success / or timeout
    STATE_LOCKED,                // Default regular state; E-ink masked, RFID off
    STATE_BLE_SYNCING,           // Active BLE write; locking out button presses, wait for card data to finish syncing, back to LOCKED
    STATE_RFID_TRANSMITTING,     // Auth successful; RFID broadcasting single selected card -> back to LOCKED after timeout... or successful write?
    STATE_RESET                  // Factory reset state, wipes cards/gps from NVM and resets system
} wallnet_state_t;

#define MAX_NAME_LEN 20
#define MAX_LAST_FOUR_LEN 5 // 4 digits + null terminator
#define MAX_ENC_PAN_LEN 44 // update this and 2 below once length is finalized (i can't test encryption rn)
#define MAX_ENC_CVV_LEN 24  
#define MAX_ENC_EXP_LEN 24  

// new encrypted card struct

#ifndef CARD_RECORD
#define CARD_RECORD
typedef struct {
    bool valid;
    char first_name[MAX_NAME_LEN];
    char last_name[MAX_NAME_LEN];
    char last_four[MAX_LAST_FOUR_LEN];
    
    uint8_t enc_pan[MAX_ENC_PAN_LEN];
    uint8_t enc_pan_len;
    
    uint8_t enc_cvv[MAX_ENC_CVV_LEN];
    uint8_t enc_cvv_len;
    
    uint8_t enc_exp[MAX_ENC_EXP_LEN];
    uint8_t enc_exp_len;
} card_record_t;

#endif // CARD_RECORD


// packed so it doesn't pad 0s and let's me keep 14 byte packet struct
struct __attribute__((__packed__)) gps_telemetry_t {
    int32_t latitude;
    int32_t longitude;
    uint32_t time_raw;
    uint8_t sats_and_fix;
    uint8_t hdop_scaled;
};

extern volatile wallnet_state_t sys_current_state;
extern volatile bool sys_is_armed;
extern volatile bool sys_is_connected;
extern volatile bool sys_is_bonded;
extern volatile uint8_t sys_active_card_idx;
extern volatile uint8_t sys_num_cards;

extern card_record_t sys_card_slots[MAX_CARDS];
extern struct gps_telemetry_t sys_current_gps_payload;
extern volatile bool sys_have_valid_gps;

extern struct k_work_q fp_workq;
extern struct k_work_delayable auth_check_work;

void wallnet_ble_init(void);
void wallnet_ble_adv_start(void);
void wallnet_ble_adv_stop(void);

void wallnet_ui_init(void);
void update_eink_display(void);

void wallnet_auth_init(void);

void wallnet_gps_init(void);
void wallnet_gps_start(void);
void wallnet_gps_stop(void);
void wallnet_ble_notify_gps(void);

void wallnet_auth_trigger(void);
int wallnet_save_cards_to_nvm(void);
int wallnet_save_fingerprint_count_to_nvm(void);

extern uint16_t sys_num_fingers;
void wallnet_enroll_trigger(void);


int BQ_init(void);
int BQ_status(void);
int BQ_ret_batt(void);
int BQ_write(uint8_t reg, uint8_t value);
extern int sys_batt_level; // 0, 1, or 2 for low, medium, high



#endif // SYSTEM_STATE_H
