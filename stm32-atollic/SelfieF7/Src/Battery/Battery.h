/*
 * Battery.h
 *
 *  Created on: 05.01.2018
 *      Author: mice
 */

#ifndef BATTERY_BATTERY_H_
#define BATTERY_BATTERY_H_

float Sharp_f;

float Amps_f;
float Volts_f;
float mAhs_drawn;
void StartBatteryManager(void const * argument);
#endif /* BATTERY_BATTERY_H_ */

uint16_t adc_raw[3];
