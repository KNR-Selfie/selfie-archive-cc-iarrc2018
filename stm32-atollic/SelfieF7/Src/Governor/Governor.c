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

float parking_depth = 10.f; // [mm]
float parking_turn_sharpness = 40.f; // [degree]
float parking_dead_fwd = 30.f; // [mm]

uint8_t lane_switching_move = 0;
uint8_t parking_move = 0;
uint8_t parking_search_move =0;
uint8_t challenge_select = 0;
uint8_t crossing_move = 0;
float place_lenght = 0;
float start_place =0;
float crossing_dist = 0;


float start_angle = 0;
float start_distance = 0;


void lane_switch_f(void);
void autonomous_task_f(void);
void semi_task_f(void);
void radio_to_actuators_f(void);
void parking_f(void);
void parking_search_f(void);

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
		set_spd = 700;
		set_pos = 1000;
		set_angle = 90;
	} else if (j_syncByte == 200) //je?eli nie istnieje Jetson <-> STM, wylacz naped (wartosc j_syncByte = 200 jest ustawiana przez TIMER10)
			{
		set_spd = 0;
	}
	switch (autonomous_task) {
	case await:
		await_f();
		break;
	case stopped:
		break;
	case lanefollower:
		break;
	case laneswitch:
		lane_switch_f();
		break;
	case crossing:
		crossing_f();
		break;
	case parkingsearch:
		parking_search_f();
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

	//testy na semi!!!
	if(autonomous_task == laneswitch)
		lane_switch_f();
}
void radio_to_actuators_f(void)
{
	if (((FutabaChannelData[1] - 1027) > 50) || ((FutabaChannelData[1] - 1027) < -50))
		set_spd = (1840 * (FutabaChannelData[1] - 1027) / (1680 - 368));
	else set_spd = 0;

	dutyServo = (servo_middle + 600 * (FutabaChannelData[3] - 1000) / (1921 - 80));
}
void parking_f(void) {
	float steering = 0, velocity = 0;
	if (parking_move == 0) {
		sidesignals = SIDETURN_RIGHT;
		start_angle = CumulativeYaw;
		++parking_move;
		velocity = -0;
		parking_angle = steering;
		set_spd = velocity;
		osDelay(100);
	}
	if (parking_move == 1) {
		sidesignals = SIDETURN_RIGHT;
		++parking_move;
		velocity = 0;
		parking_angle = steering;
		set_spd = velocity;
		osDelay(100);
	}
	if (parking_move == 2) {
		sidesignals = SIDETURN_RIGHT;
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
		sidesignals = SIDETURN_RIGHT;
		steering = (CumulativeYaw - start_angle);
		velocity = -500;
		if ((fwdRoad - start_distance) < -50) {
			++parking_move;
			start_angle = CumulativeYaw;
		}
	}
	if (parking_move == 4) {
		sidesignals = SIDETURN_RIGHT;
		steering = -90.f;
		velocity = -500;
		if ((CumulativeYaw - start_angle) > 35
				|| (CumulativeYaw - start_angle) < -35 || flags[1] ==1) {

			parking_angle = 0.0f;
			set_spd = 0.0f;
			sidesignals = SIDETURN_EMERGENCY;
			osDelay(1200);
			start_angle = CumulativeYaw;
			++parking_move;
		}
	}
	if (parking_move == 5) {
		sidesignals = SIDETURN_LEFT;
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
		sidesignals = SIDETURN_LEFT;
		steering = -(CumulativeYaw - start_angle);
		velocity = 500;
		if ((fwdRoad - start_distance) < 50) {
			++parking_move;
			start_angle = CumulativeYaw;
		}
	}
	if (parking_move == 7) {
		sidesignals = SIDETURN_LEFT;
		steering = 90.f;
		velocity = 500;
		if ((CumulativeYaw - start_angle) > 35
				|| (CumulativeYaw - start_angle) < -35) {

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

//funkcja od wyprzedzania. Wywolywana w trybach autonomous i semi. Rozpatrywana po przebiciu przez prog w Czujniki.c
void lane_switch_f(void){
	if(flags[0] && lane_switching_move == 1){
	HAL_GPIO_WritePin(Change_Lane_GPIO_Port,Change_Lane_Pin, GPIO_PIN_RESET);
	lane_switching_move = 2;
	}
	//szukamy konca przeszkody
	else if(!(flags[0]) && lane_switching_move == 2){
		HAL_GPIO_WritePin(Change_Lane_GPIO_Port,Change_Lane_Pin, GPIO_PIN_SET);
		HAL_TIM_Base_Start_IT(&htim11);
	}
}
//moze nastapic ponowne wystawienie flagi do odroida. Flaga gaszona po 0.5s z timera. Timer powraca stan autonomous_task do tradycyjnego lanefollowera.


//
////funkcja od wyprzedzania. Wywolywana w trybach autonomous i semi. Rozpatrywana po przebiciu przez prog w Czujniki.c
//void lane_switch_f(void){
//	static int count_vl_objects = 0;
//
// if(lane_switching_move == 1){
//	if(flags[0] && count_vl_objects == 0){
//		count_vl_objects = 1;
//	HAL_GPIO_WritePin(Change_Lane_GPIO_Port,Change_Lane_Pin, GPIO_PIN_RESET);
//	}
//	//szukamy konca przeszkody
//	else if(!(flags[0]) && count_vl_objects == 1)
//		lane_switching_move = 2;
//}
////moze nastapic ponowne wystawienie flagi do odroida. Flaga gaszona po 0.5s z timera. Timer powraca stan autonomous_task do tradycyjnego lanefollowera.
//else if(lane_switching_move == 2){
//	count_vl_objects = 0;
//	HAL_GPIO_WritePin(Change_Lane_GPIO_Port,Change_Lane_Pin, GPIO_PIN_SET);
//	HAL_TIM_Base_Start_IT(&htim11);
//}
//
//}

void parking_search_f(void) {

	//wykrycie poczatku pudelka
	if (parking_search_move == 0 && flags[0] == 1) {
		++parking_search_move;
	}
	//wykrycie konca pudelka - rozpoczecie mierzenia miejsca
	if (parking_search_move == 1 && flags[0] == 0) {
		start_place = fwdRoad;
		++parking_search_move;
	}
	//wykrycie kolejnego pudelka
	if (parking_search_move == 2 && flags[0] == 1) {
		++parking_search_move;
		place_lenght = fwdRoad - start_place;
		if (place_lenght > 200) {
			autonomous_task = parking;
		} else
			parking_search_move = 0;
	}
}

void await_f(void)
{
    set_spd =0;
    if (HAL_GPIO_ReadPin(Parking_Button_GPIO_Port, Parking_Button_Pin) == GPIO_PIN_SET)
    {
        autonomous_task=lanefollower;
        challenge_select = 1;
    }
    else if(HAL_GPIO_ReadPin(Obstacle_Button_GPIO_Port,Obstacle_Button_Pin) == GPIO_PIN_SET){

        autonomous_task=lanefollower;
    }
}


void crossing_f(void) {
	//zatrzymanie
	if (crossing_move == 0) {
		set_spd = 0;
		osDelay(3000);
		++crossing_move;
	}
	//podjechanie metr do przodu
	if (crossing_move == 1) {
		crossing_dist = fwdRoad;
		set_spd = 500;
		set_pos = 1000;
		if (fwdRoad - crossing_dist > 1000) {
			crossing_move=0;
			HAL_GPIO_TogglePin(Vision_Reset_GPIO_Port, Vision_Reset_Pin);
			osDelay(70);
			HAL_GPIO_TogglePin(Vision_Reset_GPIO_Port, Vision_Reset_Pin);
			autonomous_task = lanefollower;

		}
	}

}

