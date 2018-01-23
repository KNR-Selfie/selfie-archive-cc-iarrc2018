/*
 * Enc.h
 *
 *  Created on: 14.12.2017
 *      Author: Michal
 */

#ifndef ENC_ENC_H_
#define ENC_ENC_H_

#include "stm32f7xx_hal.h"
#include "Filtering.h"
#include "cmsis_os.h"

extern volatile int16_t vleft; //(mm/s)
extern volatile int16_t vright; //(mm/s)
extern volatile int16_t vfwd; //(mm/s)

extern volatile float leftRoad; // mm
extern volatile float rightRoad; // mm
extern volatile float fwdRoad ; // mm

extern pt1Filter_t DtermLPF;

extern osSemaphoreId PIDSemaphoreHandle;

void encodersRead(void);
void encodersReset(void);
void StartEncTask(void const * argument);

#endif /* ENC_ENC_H_ */
