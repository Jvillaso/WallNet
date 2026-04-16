
#include "buzzer.h"
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/sys/printk.h>

uint8_t sys_buzzer_state = 0;

// Define gpio device used for buzzer (node label may vary per board)
const struct device *gpio_dev = DEVICE_DT_GET(DT_NODELABEL(gpio0));

void buzzer_init(void) {
    if (!device_is_ready(gpio_dev)) {
        printk("Error: GPIO device not ready!\n");
        return;
    }
    gpio_pin_configure(gpio_dev, BUZZER_PIN, GPIO_OUTPUT);
}

void buzzer_toggle(void) {
    sys_buzzer_state = (sys_buzzer_state == 0) ? 1 : 0;
    gpio_pin_set(gpio_dev, BUZZER_PIN, sys_buzzer_state);
}