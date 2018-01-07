/*
 * Czujniki.h
 *
 *  Created on: 16.12.2017
 *      Author: Michal
 */

#ifndef VL53L0X_CZUJNIKI_H_
#define VL53L0X_CZUJNIKI_H_

#include "stm32f7xx_hal.h"

uint16_t range;

void StartCzujnikiTask(void const * argument);

#endif /* VL53L0X_CZUJNIKI_H_ */
