#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

#define GPS_UART     DEVICE_DT_GET(DT_NODELABEL(uart0))
#define NMEA_MAX_LEN 100

static const struct device *gps_uart = GPS_UART;
static char line_buf[NMEA_MAX_LEN];
static int  line_pos = 0;

static void uart_cb(const struct device *dev, void *user_data)
{
    printk("UART callback triggered\n");
    uint8_t c;
    if (!uart_irq_update(dev) || !uart_irq_rx_ready(dev)) return;

    while (uart_fifo_read(dev, &c, 1) == 1) {
        printk("%d\n", c);
        // if (c == '\n' && line_pos > 0) {
        //     line_buf[line_pos] = '\0';
        //     printk("%s\n", line_buf);
        //     line_pos = 0;
        // } else if (c != '\r' && line_pos < NMEA_MAX_LEN - 1) {
        //     line_buf[line_pos++] = c;
        // }
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
    printk("GPS reader started, waiting for NMEA...\n");

    while (1) {
        k_sleep(K_FOREVER);
    }

    return 0;
}