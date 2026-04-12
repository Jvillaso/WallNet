#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define MAX_CARDS 16
#define MAX_CARDHOLDER_LEN 40
#define MAX_CARDNUMBER_LEN 24
#define MAX_EXP_LEN 8
#define MAX_CVV_LEN 8

// Main WallNet State Machine
typedef enum {
    STATE_BOOT_CHECK,            // Check for existing fingerprint on boot
    STATE_INIT_WAIT_ENROLL,      // First-time setup gate; waiting for user to start scanner w/ button press
    STATE_ENROLLING,             // Fingerprint enrollment process active; wait for success or timeout
    STATE_INIT_WAIT_PAIRING,     // Second setup gate; waiting for user to press button to start BLE advertising/paring processs
    STATE_PAIRING_ADVERTISING,   // BLE radio active; wait for BT connection and pairing success / or timeout
    STATE_LOCKED,                // Default regular state; E-ink masked, RFID off
    STATE_BLE_SYNCING,           // Active BLE write; locking out button presses, wait for card data to finish syncing, back to LOCKED
    STATE_RFID_TRANSMITTING      // Auth successful; RFID broadcasting single selected card -> back to LOCKED after timeout... or successful write?
} wallnet_state_t;

typedef struct {
    bool valid;
    char cardholder[MAX_CARDHOLDER_LEN];
    char cardnumber[MAX_CARDNUMBER_LEN];
    char exp[MAX_EXP_LEN];
    char cvv[MAX_CVV_LEN];
} card_record_t;

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

void wallnet_ble_init(void);
void wallnet_ble_adv_start(void);
void wallnet_ble_adv_stop(void);

void wallnet_ui_init(void);
void update_eink_display(void);

void wallnet_auth_init(void);
void wallnet_gps_init(void);
void wallnet_ble_notify_gps(void);

#endif // SYSTEM_STATE_H