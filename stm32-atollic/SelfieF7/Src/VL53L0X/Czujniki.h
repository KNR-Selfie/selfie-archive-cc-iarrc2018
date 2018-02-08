/*
 * Czujniki.h
 *
 *  Created on: 16.12.2017
 *      Author: Michal
 */

#ifndef VL53L0X_CZUJNIKI_H_
#define VL53L0X_CZUJNIKI_H_

#include "stm32f7xx_hal.h"
#include "cmsis_os.h"
#define VLX_SENSOR_COUNT 8

extern xSemaphoreHandle i2c2_semaphore;

uint16_t range[VLX_SENSOR_COUNT];
uint16_t lane_change_treshold;

uint16_t flags[3];

void StartCzujnikiTask(void const * argument);

#endif /* VL53L0X_CZUJNIKI_H_ */
