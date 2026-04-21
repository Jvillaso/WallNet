#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/logging/log.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/settings/settings.h>

#include "system_state.h"

LOG_MODULE_REGISTER(wallnet_gps, LOG_LEVEL_INF);

#define I2C_NODE DT_NODELABEL(i2c0)
#define GPS_I2C_ADDR 0x42

#define GPS_CHUNK_MAX 32
#define GPS_LINE_BUF_SIZE 128

static char line_buf[GPS_LINE_BUF_SIZE];
static int line_idx = 0;

static const struct device *i2c_dev;

// both delayable now
static struct k_work_delayable gps_i2c_read_work;
static struct k_work_delayable gps_ble_notify_work;

static void get_nmea_field(const char *sentence, int field_num, char *out_buf, int max_len) {
    int current_field = 0, i = 0, out_idx = 0;
    if (max_len <= 0) return;
    out_buf[0] = '\0';

    while (sentence[i] != '\0' && sentence[i] != '*') {
        if (sentence[i] == ',') {
            current_field++;
        } else if (current_field == field_num && out_idx < (max_len - 1)) {
            out_buf[out_idx++] = sentence[i];
        } else if (current_field > field_num) {
            break;
        }
        i++;
    }
    out_buf[out_idx] = '\0';
}

static double convert_to_decimal(const char *nmea_coord, const char *direction) {
    if (nmea_coord[0] == '\0') return 0.0;
    double raw = atof(nmea_coord);
    int degrees = (int)(raw / 100);
    double minutes = raw - (degrees * 100);
    double decimal = degrees + (minutes / 60.0);
    if (direction[0] == 'S' || direction[0] == 'W') decimal *= -1.0;
    return decimal;
}

static void handle_gga_sentence(const char *sentence) {
    char time_str[16], lat_raw[16], ns[2], lon_raw[16], ew[2];
    char fix_str[4], sats_str[4], hdop_str[8];

    get_nmea_field(sentence, 1, time_str, sizeof(time_str));
    get_nmea_field(sentence, 2, lat_raw, sizeof(lat_raw));
    get_nmea_field(sentence, 3, ns, sizeof(ns));
    get_nmea_field(sentence, 4, lon_raw, sizeof(lon_raw));
    get_nmea_field(sentence, 5, ew, sizeof(ew));
    get_nmea_field(sentence, 6, fix_str, sizeof(fix_str));
    get_nmea_field(sentence, 7, sats_str, sizeof(sats_str));
    get_nmea_field(sentence, 8, hdop_str, sizeof(hdop_str));

    bool got_valid_fix = !(fix_str[0] == '0' || fix_str[0] == '\0');

    if (!got_valid_fix) {
        // LOG_WRN("Status: No Fix Yet");
        sys_have_valid_gps = false;
        return;
    }

    double latitude = convert_to_decimal(lat_raw, ns);
    double longitude = convert_to_decimal(lon_raw, ew);
    uint8_t fix_val = (uint8_t)atoi(fix_str);
    uint8_t sats_val = (uint8_t)atoi(sats_str);
    float hdop_val = (float)atof(hdop_str);

    // global array
    sys_current_gps_payload.latitude = (int32_t)(latitude * 10000000.0);
    sys_current_gps_payload.longitude = (int32_t)(longitude * 10000000.0);
    sys_current_gps_payload.time_raw = (uint32_t)atoi(time_str);
    sys_current_gps_payload.sats_and_fix = (uint8_t)(((sats_val > 31U ? 31U : sats_val) << 3) | (fix_val & 0x07U));
    sys_current_gps_payload.hdop_scaled = (uint8_t)(hdop_val * 10.0f);
    
    sys_have_valid_gps = true;
    // LOG_WRN("Valid Fix: Lat %.6f | Lon %.6f", latitude, longitude);
}

static void process_rx_bytes(const uint8_t *buf, size_t len) {
    for (size_t i = 0; i < len; i++) {
        uint8_t c = buf[i];
        // ending byte can be 0xFF or \r\n, ignore both
        if (c == 0xFF || c == '\r') continue;


        if (c == '$') {
            line_idx = 0;
            line_buf[line_idx++] = (char)c;
            continue;
        }

        if (line_idx == 0) continue;

        if (line_idx < (GPS_LINE_BUF_SIZE - 1)) {
            line_buf[line_idx++] = (char)c;
        } else {
            line_idx = 0;
            continue;
        }

        if (c == '\n') {
            line_buf[line_idx] = '\0';
            // LOG_INF("Raw NMEA: %s", line_buf);
            if (strncmp(line_buf, "$GNGGA", 6) == 0 || strncmp(line_buf, "$GPGGA", 6) == 0) {
                handle_gga_sentence(line_buf);
            }
            line_idx = 0;
        }
    }
}

// work thread since relatively slow
static void gps_i2c_read_worker(struct k_work *work) {
    uint8_t rx_buf[GPS_CHUNK_MAX];
    bool data_remaining = true;

    // Drain SAM-M10Q hardware buffer 
    while (data_remaining) {
        int err = i2c_read(i2c_dev, rx_buf, sizeof(rx_buf), GPS_I2C_ADDR);
        
        if (err == 0) {
            // u-blox chips pad empty I2C reads with 0xFF
            // If the first byte is 0xFF, the buffer is empty
            if (rx_buf[0] == 0xFF) {
                data_remaining = false;
            } else {
                process_rx_bytes(rx_buf, sizeof(rx_buf));
            }
        } else {
            LOG_ERR("I2C read failed (%d)", err);
            data_remaining = false;
        }
        
        // yield to other threads, i2c slow
        k_yield(); 
    }

    // Check again in exactly 1 second
    k_work_reschedule(&gps_i2c_read_work, K_SECONDS(1));
}

static void gps_ble_notify_timer(struct k_work *work) {
    if (sys_is_connected && sys_have_valid_gps) {
        wallnet_ble_notify_gps();
    }
    k_work_reschedule(&gps_ble_notify_work, K_SECONDS(30));
}

static int gps_settings_set(const char *name, size_t len, settings_read_cb read_cb, void *cb_arg) {
    // looking for "gps/last_fix" key
    if (strcmp(name, "last_fix") == 0) {
        if (len != sizeof(sys_current_gps_payload)) return -EINVAL;
        
        int rc = read_cb(cb_arg, &sys_current_gps_payload, sizeof(sys_current_gps_payload));
        if (rc >= 0) {
            sys_have_valid_gps = true; 
            LOG_WRN("Restored historical GPS coordinates from NVM.");
        }
        return 0;
    }
    return -ENOENT;
}

static struct settings_handler gps_conf = {
    .name = "gps",
    .h_set = gps_settings_set
};


void wallnet_gps_start(void) {
    LOG_INF("Starting GPS Tracking.");
    // Instantly wake up the I2C read thread
    k_work_reschedule(&gps_i2c_read_work, K_NO_WAIT);
    // Start the 30-second BLE blast timer
    k_work_reschedule(&gps_ble_notify_work, K_SECONDS(30));
}

void wallnet_gps_stop(void) {
    LOG_INF("Stopping GPS Tracking.");
    // Cancel the timers so the nRF52 can sleep
    k_work_cancel_delayable(&gps_i2c_read_work);
    k_work_cancel_delayable(&gps_ble_notify_work);
}

void wallnet_gps_init(void) {
    
    settings_register(&gps_conf);
    i2c_dev = DEVICE_DT_GET(I2C_NODE);
    if (!device_is_ready(i2c_dev)) {
        LOG_ERR("I2C device not ready! Check app.overlay");
        return;
    }

    // Initialize both delayable workers
    k_work_init_delayable(&gps_i2c_read_work, gps_i2c_read_worker);
    k_work_init_delayable(&gps_ble_notify_work, gps_ble_notify_timer);

    // Start the 1Hz read loop 
    k_work_reschedule(&gps_i2c_read_work, K_NO_WAIT);

    // Start the 30-second notification loop
    k_work_reschedule(&gps_ble_notify_work, K_SECONDS(30));
    
    LOG_WRN("GPS Subsystem Initialized");
}
