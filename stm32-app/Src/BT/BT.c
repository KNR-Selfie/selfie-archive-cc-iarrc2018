/*
 * BT.c
 *
 *  Created on: 19.12.2017
 *      Author: Michal
 */

#include "BT.h"
#include "cmsis_os.h"

osSemaphoreDef(BTSemaphore);


void StartBTTask(void const * argument) {
	osSemaphoreId osSemaphore = osSemaphoreCreate(osSemaphore(BTSemaphore), 1);
	osSemaphoreWait(osSemaphore, osWaitForever);
	while (1) {

	}
}
