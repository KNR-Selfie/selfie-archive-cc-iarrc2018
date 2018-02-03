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

xSemaphoreHandle i2c2_semaphore = NULL;

#define EXPANDER_ADRESS 0x40

void StartCzujnikiTask(void const * argument) {
	vSemaphoreCreateBinary(i2c2_semaphore);
	osSemaphoreWait(i2c2_semaphore, osWaitForever);
	MX_I2C2_Init();

	lane_change_treshold = 600;

	static uint8_t expander_bits[5];
	expander_bits[0] = 0x00;
	HAL_I2C_Master_Transmit(&hi2c2, EXPANDER_ADRESS, expander_bits, 1, 5);
	osDelay(10);

	for (int sensor = 0; sensor < VLX_SENSOR_COUNT; sensor++) {

		expander_bits[0] |= (1 << sensor);
		HAL_I2C_Master_Transmit(&hi2c2, EXPANDER_ADRESS, expander_bits, 1, 5);
		VLX_CURRENT_ADRESS = ADDRESS_DEFAULT + sensor*2 + 10;
		osDelay(5);
		VL53L0X_init();
		osDelay(100);
		VL53L0X_startContinuous(0);
		osDelay(100);
	}
	while (1) {
		for (int sensor = 0; sensor < VLX_SENSOR_COUNT; sensor++) {
			VLX_CURRENT_ADRESS = ADDRESS_DEFAULT + sensor*2 + 10;
			range[sensor] = readRangeContinuousMillimeters();
			osDelay(10);
		}
		if(range[7] < (lane_change_treshold + 100))
			HAL_GPIO_WritePin(Change_Lane_GPIO_Port,Change_Lane_Pin, GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(Change_Lane_GPIO_Port,Change_Lane_Pin, GPIO_PIN_RESET);
	}

}
