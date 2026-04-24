#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/logging/log.h>
#include "system_state.h"

LOG_MODULE_REGISTER(wallnet_bq, LOG_LEVEL_INF);

#define I2C_NODE DT_NODELABEL(i2c1)
#define BQ_ADDR  0x6A

static const struct device *i2c_dev;
static struct k_work_delayable bq_read_work;

/* ---------- read ---------- */
static int BQ_read_reg(uint8_t reg, uint8_t *data)
{
	return i2c_write_read(i2c_dev, BQ_ADDR, &reg, 1, data, 1);
}

/* ---------- battery processing ---------- */

static int BQ_get_batt_level(void)
{
	uint8_t temp;

	if (BQ_read_reg(0x0E, &temp)) {
		LOG_ERR("BQ read failed you dum b chud");
		return -1;
	}

	uint8_t val = temp & 0x7F;
	int mv = 2304 + (val * 20);

	if (mv < 3400) return 0;
	else if (mv < 3800) return 1;
	else return 2;
}

static void bq_read_worker(struct k_work *work) {

	wallnet_gps_stop(); 

	int batt_level = BQ_get_batt_level();

	sys_batt_level = batt_level;
}



/* ---------- init ---------- */

int BQ_init(void)
{
    i2c_dev = DEVICE_DT_GET(I2C_NODE);

    if (!device_is_ready(i2c_dev)) {
        return -1;
    }

    BQ_write(0x02, 0x3C);
    BQ_write(0x04, 0x08);

    k_work_init_delayable(&bq_read_work, bq_read_worker);
    k_work_reschedule(&bq_read_work, K_NO_WAIT);

    return 0;
}

int BQ_write(uint8_t reg, uint8_t value)
{
    uint8_t buf[2] = {reg, value};
    return i2c_write(i2c_dev, buf, 2, 0x6A);
}