#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/led.h>
#include <zephyr/logging/log.h>
#include "buzzer.h"

uint8_t sys_buzzer_state = 0;

/* IMPORTANT: use pwm-leds PARENT node */
#define BUZZER_NODE DT_NODELABEL(buzzer_pwm)

static const struct device *led_dev = DEVICE_DT_GET(BUZZER_NODE);

void buzzer_init(void) {
    if (!device_is_ready(led_dev)) {
        printk("Buzzer not ready!\n");
        return;
    }

    led_off(led_dev, 0);
}

void buzzer_toggle(void) {
    sys_buzzer_state = !sys_buzzer_state;

    if (sys_buzzer_state) {
        led_set_brightness(led_dev, 0, 128);
        printk("Buzzer ON\n");
    } else {
        led_off(led_dev, 0);
        printk("Buzzer OFF\n");
    }
}