/*
 * Governor.c
 *
 *  Created on: 14.01.2018
 *      Author: mice
 */

#include "Governor.h"

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#include "main.h"
#include "usart.h"
#include "gpio.h"


#include "Gyro.h"
#include "Lighting.h"
#include "Czujniki.h"
#include "Futaba.h"
#include "Czujniki.h"
#include "tim.h"
#include "Steering.h"

uint8_t parking_move = 0;
float start_angle = 0;
float start_distance = 0;

void autonomous_task_f(void);
void semi_task_f(void);
void radio_to_actuators_f(void);
void parking_f(void);

void StartGovernorTask(void const * argument) {

	HAL_GPIO_TogglePin(Vision_Reset_GPIO_Port, Vision_Reset_Pin);
	osDelay(70);
	HAL_GPIO_TogglePin(Vision_Reset_GPIO_Port, Vision_Reset_Pin);
	osDelay(10);
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
		osDelay(5);
	}
}
void autonomous_task_f(void) {

	HAL_TIM_Base_Start_IT(&htim10); // timer od sprawdzania komunikacji
	if (j_syncByte == 255) {
		set_spd = 920;
		set_pos = 1000;
		set_angle = 90;
	} else if (j_syncByte == 200) //je?eli nie istnieje Jetson <-> STM, wylacz naped (wartosc j_syncByte = 200 jest ustawiana przez TIMER10)
			{
		set_spd = 0;
	}
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
		parking_f();
		break;
	}
}
void semi_task_f(void)
{
	if (((FutabaChannelData[1] - 1027) > 50)
			|| ((FutabaChannelData[1] - 1027) < -50))
		set_spd = (1840 * (FutabaChannelData[1] - 1027) / (1680 - 368));
	else
		set_spd = 0;
	set_pos = 1000;
	set_angle = 90;
}
void radio_to_actuators_f(void)
{
	if (((FutabaChannelData[1] - 1027) > 50) || ((FutabaChannelData[1] - 1027) < -50))
		set_spd = (1840 * (FutabaChannelData[1] - 1027) / (1680 - 368));
	else set_spd = 0;

	dutyServo = (servo_middle + 600 * (FutabaChannelData[3] - 1000) / (1921 - 80));
}
void parking_f(void) {
	sidesignals = SIDETURN_RIGHT;
	float steering = 0, velocity = 0;
	if (parking_move == 0) {
		start_angle = CumulativeYaw;
		++parking_move;
		velocity = -0;
		parking_angle = steering;
		set_spd = velocity;
		osDelay(100);
	}
	if (parking_move == 1) {
		++parking_move;
		velocity = 0;
		parking_angle = steering;
		set_spd = velocity;
		osDelay(100);
	}
	if (parking_move == 2) {
		steering = 90.f;
		velocity = -500;
		if ((CumulativeYaw - start_angle) > 35
				|| (CumulativeYaw - start_angle) < -35) {
			++parking_move;
			start_distance = fwdRoad;
			start_angle = CumulativeYaw;
		}
	}
	if (parking_move == 3) {

		steering = (CumulativeYaw - start_angle);
		velocity = -500;
		if ((fwdRoad - start_distance) < -50) {
			++parking_move;
			start_angle = CumulativeYaw;
		}
	}
	if (parking_move == 4) {
		steering = -90.f;
		velocity = -500;
		if ((CumulativeYaw - start_angle) > 35
				|| (CumulativeYaw - start_angle) < -35 || range[0] < 50) {

			parking_angle = 0.0f;
			set_spd = 0.0f;
			sidesignals = SIDETURN_EMERGENCY;
			osDelay(1200);
			sidesignals = SIDETURN_LEFT;
			start_angle = CumulativeYaw - start_angle - 35;
			++parking_move;
		}
	}
	if (parking_move == 5) {
			steering = -90.f;
			velocity = 500;
			if ((CumulativeYaw - start_angle) > 35
					|| (CumulativeYaw - start_angle) < -35) {
				++parking_move;
				start_distance = fwdRoad;
				start_angle = CumulativeYaw;
			}
		}
		if (parking_move == 6) {

			steering = (CumulativeYaw - start_angle);
			velocity = 500;
			if ((fwdRoad - start_distance) < -50) {
				++parking_move;
				start_angle = CumulativeYaw;
			}
		}
		if (parking_move == 7) {
			steering = 90.f;
			velocity = 500;
			if ((CumulativeYaw - start_angle) > 35
					|| (CumulativeYaw - start_angle) < -35 || range[0] < 50) {

				parking_angle = 0.0f;
				set_spd = 0.0f;
				sidesignals = SIDETURN_NONE;
				parking_move = 0;
				HAL_GPIO_TogglePin(Vision_Reset_GPIO_Port, Vision_Reset_Pin);
				osDelay(70);
				HAL_GPIO_TogglePin(Vision_Reset_GPIO_Port, Vision_Reset_Pin);
				autonomous_task = lanefollower;
			}
		}
	parking_angle = steering;
	set_spd = velocity;
}

