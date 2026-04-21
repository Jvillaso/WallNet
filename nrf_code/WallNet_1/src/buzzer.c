#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/logging/log.h>
#include "buzzer.h"


// Buzzer state (0 = OFF, 1 = ON)
uint8_t sys_buzzer_state = 0;

const struct pwm_dt_spec pwm_buzzer_ch0 = {
    .dev = DEVICE_DT_GET(DT_NODELABEL(pwm0)),
    .channel = 0,
    .period = 0,
    .flags = 0
};



void buzzer_init(void) {
    if (!device_is_ready(pwm_buzzer_ch0.dev)) {
        printk("PWM device not ready\n");
        return;
    }
  
    
    /* Start OFF - set 0% duty cycle for both channels */
    pwm_set_dt(&pwm_buzzer_ch0, PWM_USEC(250), 0);
    sys_buzzer_state = false;
}


void buzzer_toggle(void) {
    if (!sys_buzzer_state)
    {
        /* Start PWM output for 4kHz signal - 250us period, 125us pulse (50% duty) for both channels */
        pwm_set_dt(&pwm_buzzer_ch0, PWM_USEC(250), PWM_USEC(125));
        sys_buzzer_state = true;
        printk("Buzzer ON\n");
    }
    else
    {
        /* Stop PWM output - set 0% duty cycle for both channels */
        pwm_set_dt(&pwm_buzzer_ch0, PWM_USEC(250), 0);
        sys_buzzer_state = false;
        printk("Buzzer OFF\n");
    }
}