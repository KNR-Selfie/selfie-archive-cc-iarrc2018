/*
 * Enc.h
 *
 *  Created on: 14.12.2017
 *      Author: Michal
 */

#ifndef ENC_ENC_H_
#define ENC_ENC_H_

#include "stm32f7xx_hal.h"

extern volatile int16_t vleft; //(mm/s)
extern volatile int16_t vright; //(mm/s)
extern volatile int16_t vfwd; //(mm/s)

extern volatile int32_t leftRoad; // mm
extern volatile int32_t rightRoad; // mm
extern volatile int32_t fwdRoad ; // mm
int16_t wheel_pid(float kp, float ki, float kd, int16_t setfwd);

void StartEncTask(void const * argument);

#endif /* ENC_ENC_H_ */
