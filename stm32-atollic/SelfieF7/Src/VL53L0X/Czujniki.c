/*
 * Czujniki.c
 *
 *  Created on: 16.12.2017
 *      Author: Michal
 */

#include "Czujniki.h"
#include "cmsis_os.h"
#include "math.h"
#include "tim.h"
#include "gpio.h"
#include "VL53L0X.h"



uint16_t range[4];
uint8_t adresy[4] = {102, 104, 106, 108};

void czujnikINIT(void);

void StartCzujnikiTask(void const * argument)
{
	/*
	HAL_GPIO_WritePin(XSHUT0_GPIO_Port, XSHUT0_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(XSHUT1_GPIO_Port, XSHUT1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(XSHUT2_GPIO_Port, XSHUT2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(XSHUT3_GPIO_Port, XSHUT3_Pin, GPIO_PIN_RESET);

	osDelay(1);
	czujnikINIT();
	VL53L0X_setAddress(adresy[0]);

	HAL_GPIO_WritePin(XSHUT1_GPIO_Port, XSHUT1_Pin, GPIO_PIN_SET);
	osDelay(1);
	czujnikINIT();
	VL53L0X_setAddress(adresy[1]);

	HAL_GPIO_WritePin(XSHUT2_GPIO_Port, XSHUT2_Pin, GPIO_PIN_SET);
	osDelay(1);
	czujnikINIT();
	VL53L0X_setAddress(adresy[2]);

	HAL_GPIO_WritePin(XSHUT3_GPIO_Port, XSHUT3_Pin, GPIO_PIN_SET);
	osDelay(1);
	czujnikINIT();
	VL53L0X_setAddress(adresy[3]);
	*/
	while (1) {
		osDelay(5000);
//		address = adresy[0];
//		range[0] = VL53L0X_readRangeSingleMillimeters();
//
//		address = adresy[1];
//		range[1] = VL53L0X_readRangeSingleMillimeters();
//
//		address = adresy[2];
//		range[2] = VL53L0X_readRangeSingleMillimeters();
//
//		address = adresy[3];
//		range[3] = VL53L0X_readRangeSingleMillimeters();

	}

}

void czujnikINIT(void)
{
	VL53L0X_init(true);
	VL53L0X_setTimeout(500);
	VL53L0X_setSignalRateLimit(0.1);
	VL53L0X_setVcselPulsePeriod(VL53L0X_VcselPeriodPreRange, 18);
	VL53L0X_setVcselPulsePeriod(VL53L0X_VcselPeriodFinalRange, 14);
	VL53L0X_setMeasurementTimingBudget(30000);
}
