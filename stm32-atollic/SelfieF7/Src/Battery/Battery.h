/*
 * Battery.h
 *
 *  Created on: 05.01.2018
 *      Author: mice
 */

#ifndef BATTERY_BATTERY_H_
#define BATTERY_BATTERY_H_

extern float Amps_f;
extern float Volts_f;
float mAhs_drawn;
void StartBatteryManager(void const * argument);
#endif /* BATTERY_BATTERY_H_ */
