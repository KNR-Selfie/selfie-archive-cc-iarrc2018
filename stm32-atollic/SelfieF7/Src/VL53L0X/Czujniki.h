/*
 * Czujniki.h
 *
 *  Created on: 16.12.2017
 *      Author: Michal
 */

#ifndef VL53L0X_CZUJNIKI_H_
#define VL53L0X_CZUJNIKI_H_

#include "cmsis_os.h"
extern uint8_t address;

void StartCzujnikiTask(void const * argument);

#endif /* VL53L0X_CZUJNIKI_H_ */
