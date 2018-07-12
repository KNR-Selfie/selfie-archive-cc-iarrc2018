/*
 * Battery.c
 *
 *  Created on: 05.01.2018
 *      Author: mice
 */
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "Battery.h"
#include "Filtering.h"

#include "cmsis_os.h"
#include "math.h"
#include "adc.h"

extern osThreadId BatteryManagerHandle;
xSemaphoreHandle ADCSemaphore = NULL;

float Amps_raw = 0;
float Amps_f = 0;
float Sharp_raw = 0;
float Sharp_f = 0;

float dT = 41.f / 375000.f;

filter_t amps_filter;
filter_t SharpLPF;

void StartBatteryManager(void const * argument){
	MX_ADC1_Init();
	vSemaphoreCreateBinary(ADCSemaphore);
	osSemaphoreWait(ADCSemaphore, osWaitForever);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) adc_raw, 3);
	lpf_filter_init(&amps_filter, 20, 1.f/dT);
	lpf_filter_init(&SharpLPF, 3.f, 1.f/dT);
	mAhs_drawn = 0;

	while(1)
	{
		osDelay(500);
//		osSemaphoreWait(ADCSemaphore, osWaitForever);

	}
}
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	if (hadc->Instance == hadc1.Instance) {
//		osSemaphoreRelease(ADCSemaphore);
		Amps_raw = (float)adc_raw[1] * 3.05f / 4095.f *20.f;
		Amps_f = filter_apply(&amps_filter, Amps_raw);
		Sharp_raw = (float)adc_raw[2] * 3.05f / 4095.f;
		Sharp_f = filter_apply(&SharpLPF, Sharp_raw);
		mAhs_drawn += Amps_raw *41.f / 375.f / 3600.f;

		Volts_f = (float)adc_raw[0] * 3.05f / 4095.f * 11;
	}
}
