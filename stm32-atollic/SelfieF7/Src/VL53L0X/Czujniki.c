/*
 * Czujniki.c
 *

 */

#include "Czujniki.h"
#include "VL53L0X.h"
#include "cmsis_os.h"
#include "tim.h"
#include "i2c.h"
#include "gpio.h"

#define EXPANDER_ADRESS 0x40

void StartCzujnikiTask(void const * argument) {
	MX_I2C2_Init();

	static uint8_t expander_bits = 0x00;
	HAL_I2C_Master_Transmit(&hi2c2, EXPANDER_ADRESS, &expander_bits, 1, 5);
	osDelay(5);

	for (int sensor = 0; sensor < VLX_SENSOR_COUNT; sensor++) {

		expander_bits |= (1 << sensor);
		HAL_I2C_Master_Transmit(&hi2c2, EXPANDER_ADRESS, &expander_bits, 1, 5);
		osDelay(5);

		VLX_CURRENT_ADRESS = ADDRESS_DEFAULT + sensor + 1;
		if (VL53L0X_init() == true) {

		} else
			;
		osDelay(5);

	}
	while (1) {
		for (int sensor = 0; sensor < VLX_SENSOR_COUNT; sensor++) {
			VLX_CURRENT_ADRESS = ADDRESS_DEFAULT + sensor + 1;
			range[sensor] = readRangeSingleMillimeters();
			osDelay(33 / VLX_SENSOR_COUNT);
		}
	}

}
