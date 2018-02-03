/*
 * Driving.c
 *
 *  Created on: 14.01.2018
 *      Author: mice
 */

#include "Driving.h"

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#include "main.h"
#include "usart.h"
#include "gpio.h"


#include "Gyro.h"
#include "Enc.h"
#include "Lighting.h"
#include "Czujniki.h"
#include "MotorControl.h"

extern float set_spd;
extern float actualSpeed;
extern float pid_speed;

//informacja o flagach z przerwania odroida
extern int ParkingFlag;
extern int CrossFlag;

void autonomous_task_f(void);
void semi_task_f(void);
void radio_to_actuators_f(void);

void StartGovernorTask(void const * argument) {


	while (1) {

		switch (driving_state) {
		case disarmed:
			break;
		case paused:
			break;
		case autonomous:
			autonomous_task_f();
			break;
		case semi:
			semi_task_f();
			break;
		case fullcontrol:
			radio_to_actuators_f();
			break;
		}
		osDelay(2);
	}
}
void autonomous_task_f(void) {
	switch (autonomous_task) {
	case await:
		break;
	case stopped:
		break;
	case lanefollower:
		break;
	case laneswitch:
		break;
	case crossing:
		break;
	case parking:
		break;
	}
}
void semi_task_f(void)
{

}
void radio_to_actuators_f(void)
{

}
