/*
 * Czujniki.c
 *

 */

#include "Czujniki.h"
#include "VL53L0X.h"
#include "cmsis_os.h"
#include "tim.h"
#include "gpio.h"



void StartCzujnikiTask(void const * argument)
{
	if (VL53L0X_init() == true) {

	} else
		;

	while (1) {
		osDelay(33);
		range = readRangeSingleMillimeters();
	}

}
