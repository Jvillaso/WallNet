#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <string.h>
#include <stdlib.h>

#define GPS_UART      DEVICE_DT_GET(DT_NODELABEL(uart0))
#define NMEA_MAX_LEN  100

static const struct device *gps_uart = GPS_UART;
static char line_buf[NMEA_MAX_LEN];
static int  line_pos = 0;

static volatile bool new_fix = false;

static char g_time[16];
static char g_lat[16];
static char g_lat_dir[4];
static char g_lon[16];
static char g_lon_dir[4];
static int  g_satellites;
static char g_altitude[16];
static int  g_fix_quality;

static bool get_field(const char *sentence, int index, char *out, size_t out_len)
{
    int field = 0;
    const char *p = sentence;

    while (*p && field < index) {
        if (*p == ',') field++;
        p++;
    }

    if (!*p) { out[0] = '\0'; return false; }

    size_t i = 0;
    while (*p && *p != ',' && *p != '*' && i < out_len - 1) {
        out[i++] = *p++;
    }
    out[i] = '\0';
    return i > 0;
}

static float nmea_to_decimal(const char *raw, const char *dir)
{
    if (raw[0] == '\0') return 0.0f;
    float raw_val = atof(raw);
    int   degrees = (int)(raw_val / 100);
    float minutes = raw_val - (degrees * 100);
    float decimal = degrees + (minutes / 60.0f);
    if (dir[0] == 'S' || dir[0] == 'W') decimal = -decimal;
    return decimal;
}

static void parse_gga(const char *sentence)
{
    char field[32];
    get_field(sentence, 1, g_time,     sizeof(g_time));
    get_field(sentence, 2, g_lat,      sizeof(g_lat));
    get_field(sentence, 3, g_lat_dir,  sizeof(g_lat_dir));
    get_field(sentence, 4, g_lon,      sizeof(g_lon));
    get_field(sentence, 5, g_lon_dir,  sizeof(g_lon_dir));
    get_field(sentence, 6, field,      sizeof(field)); g_fix_quality = atoi(field);
    get_field(sentence, 7, field,      sizeof(field)); g_satellites  = atoi(field);
    get_field(sentence, 9, g_altitude, sizeof(g_altitude));
    new_fix = true;
}

static void uart_cb(const struct device *dev, void *user_data)
{
    uint8_t c;
    if (!uart_irq_update(dev) || !uart_irq_rx_ready(dev)) return;

    while (uart_fifo_read(dev, &c, 1) == 1) {
        if (c == '\n' && line_pos > 0) {
            line_buf[line_pos] = '\0';
            if (strncmp(line_buf, "$GNGGA", 6) == 0 ||
                strncmp(line_buf, "$GPGGA", 6) == 0) {
                parse_gga(line_buf);
            }
            line_pos = 0;
        } else if (c != '\r' && line_pos < NMEA_MAX_LEN - 1) {
            line_buf[line_pos++] = c;
        }
    }
}

int main(void)
{
    if (!device_is_ready(gps_uart)) {
        printk("GPS UART not ready\n");
        return -1;
    }

    uart_irq_callback_set(gps_uart, uart_cb);
    uart_irq_rx_enable(gps_uart);
    printk("GPS reader started...\n");

    while (1) {
        k_sleep(K_SECONDS(5));

        if (!new_fix) {
            printk("--- No GPS fix yet ---\n");
            continue;
        }

        new_fix = false;

        float lat = nmea_to_decimal(g_lat, g_lat_dir);
        float lon = nmea_to_decimal(g_lon, g_lon_dir);

        char hh[3] = { g_time[0], g_time[1], '\0' };
        char mm[3] = { g_time[2], g_time[3], '\0' };
        char ss[3] = { g_time[4], g_time[5], '\0' };

        printk("=============================\n");
        printk("  Time (UTC):  %s:%s:%s\n", hh, mm, ss);
        printk("  Latitude:    %.6f\n", (double)lat);
        printk("  Longitude:   %.6f\n", (double)lon);
        printk("  Altitude:    %s m\n", g_altitude);
        printk("  Satellites:  %d\n",   g_satellites);
        printk("  Fix quality: %s\n",   g_fix_quality == 1 ? "GPS" :
                                        g_fix_quality == 2 ? "DGPS" : "None");
        printk("=============================\n");
    }

    return 0;
}