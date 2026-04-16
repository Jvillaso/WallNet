
#ifndef BUZZER_H
#define BUZZER_H

#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <stdint.h>

// Buzzer GPIO pin definition (change as needed for your board)
#define BUZZER_PIN 14  // example pin

// Buzzer state (0 = OFF, 1 = ON)
extern uint8_t sys_buzzer_state;

// GPIO device used for the buzzer
extern const struct device *gpio_dev;

void buzzer_init(void);
void buzzer_toggle(void);

#endif // BUZZER_H