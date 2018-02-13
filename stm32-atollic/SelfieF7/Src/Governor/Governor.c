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

float parking_depth = 40.f; // [mm]
float parking_turn_sharpness = 42.f; // [degree]
float parking_dead_fwd = 170.f; // [mm]

uint8_t lane_switching_move = 0;
uint8_t parking_move = 0;
uint8_t parking_search_move =0;
uint8_t challenge_select = 0;
uint8_t crossing_move = 0;
uint8_t parking_counter = 0;
float place_lenght = 0;
float start_place =0;
float crossing_dist = 0;

uint8_t hold_signals = 0;
int32_t time_then = 0;

float start_angle = 0;
float start_distance = 0;
float unfinished_angle = 0;

uint8_t oldflags_1 = 0;
float podjedz_pan = 0;
float podjedz_pan_start = 0;
float podjedz_pan_distance = -80.f;

driving_state_t old_driving_state = disarmed;

void reset_all_to_challenge(void);

void lane_switch_f(void);
void autonomous_task_f(void);
void semi_task_f(void);
void radio_to_actuators_f(void);
void parking_f(void);
void parking_search_f(void);
void crossing_f(void);
void crossing_on_parking_f(void);
void await_f(void);

void StartGovernorTask(void const * argument) {
	osDelay(100);
	HAL_GPIO_WritePin(Vision_Reset_GPIO_Port, Vision_Reset_Pin, GPIO_PIN_SET);
	osDelay(100);
	HAL_GPIO_WritePin(Vision_Reset_GPIO_Port, Vision_Reset_Pin, GPIO_PIN_RESET);

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
			if (old_driving_state != fullcontrol) {
				reset_all_to_challenge();
				}
			break;
		}
		old_driving_state = driving_state;
		osDelay(5);
	}
}
void autonomous_task_f(void) {
	if (old_driving_state == fullcontrol) {
		HAL_GPIO_WritePin(Vision_Reset_GPIO_Port, Vision_Reset_Pin,
				GPIO_PIN_SET);
		osDelay(100);
		HAL_GPIO_WritePin(Vision_Reset_GPIO_Port, Vision_Reset_Pin,
				GPIO_PIN_RESET);
	}
	HAL_TIM_Base_Start_IT(&htim10); // timer od sprawdzania komunikacji
	if (j_syncByte == 255) {
		set_spd = 700;
		set_pos = 1000;
		set_angle = 90;
	} else if (j_syncByte == 200) //je?eli nie istnieje Jetson <-> STM, wylacz naped (wartosc j_syncByte = 200 jest ustawiana przez TIMER10)
			{
		set_spd = 0;
		set_angle = 90;
		set_pos = 1000;
	}

	switch (autonomous_task) {
	case await:
		await_f();
		break;
	case stopped:
		break;
	case lanefollower:

		if (hold_signals == 1) {
			time_then = HAL_GetTick();
			hold_signals = 2;
		} else if (hold_signals == 2 && (HAL_GetTick() - time_then > 800)) {
			sidesignals = SIDETURN_NONE;
			lane_switching_move = 0;
			HAL_GPIO_WritePin(Change_Lane_GPIO_Port, Change_Lane_Pin,
					GPIO_PIN_RESET);
			hold_signals = 0;
		}

		break;
	case laneswitch:
		lane_switch_f();
		break;
	case crossing:
		crossing_f();
		break;
	case crossing_on_parking:
		crossing_on_parking_f();
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

	if (old_driving_state == fullcontrol) {
		HAL_GPIO_WritePin(Vision_Reset_GPIO_Port, Vision_Reset_Pin,
				GPIO_PIN_SET);
		osDelay(100);
		HAL_GPIO_WritePin(Vision_Reset_GPIO_Port, Vision_Reset_Pin,
						GPIO_PIN_RESET);
	}
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
	if(autonomous_task == crossing)
		crossing_f();
}
void radio_to_actuators_f(void)
{
	if (((FutabaChannelData[1] - 1027) > 50) || ((FutabaChannelData[1] - 1027) < -50))
		set_spd = (1840 * (FutabaChannelData[1] - 1027) / (1680 - 368));
	else set_spd = 0;

	dutyServo = (servo_middle - 2*servo_bandwith * (FutabaChannelData[3] - 1000) / (1921 - 80));
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
		if ((CumulativeYaw - start_angle) > parking_turn_sharpness
				|| (CumulativeYaw - start_angle) < -parking_turn_sharpness) {
			++parking_move;
			start_distance = fwdRoad;
			start_angle = CumulativeYaw;
		}
	}
	if (parking_move == 3) {
		sidesignals = SIDETURN_RIGHT;
		steering = (CumulativeYaw - start_angle);
		velocity = -500;
		if ((fwdRoad - start_distance) < -parking_depth) {
			++parking_move;
			start_angle = CumulativeYaw;
		}
	}
	if (parking_move == 4) {
			sidesignals = SIDETURN_RIGHT;
			steering = -90.f;
			velocity = -500;
			if ((CumulativeYaw - start_angle) > parking_turn_sharpness
					|| (CumulativeYaw - start_angle) < - parking_turn_sharpness || podjedz_pan < podjedz_pan_distance ) {

				parking_angle = 0.0f;
				set_spd = 0.0f;
				sidesignals = SIDETURN_EMERGENCY;
				parking_counter++;
				osDelay(1200);
				if ((CumulativeYaw - start_angle) > 0)
					unfinished_angle = parking_turn_sharpness - (CumulativeYaw - start_angle);
				else
					unfinished_angle = parking_turn_sharpness + (CumulativeYaw - start_angle);

				start_angle = CumulativeYaw;
				podjedz_pan_start = 0;
				podjedz_pan = 0;
				++parking_move;
			}
			else if (flags[1] && !oldflags_1){
				oldflags_1 = 1;
				podjedz_pan_start = fwdRoad;
			}
			else if(flags[1])
			{
				podjedz_pan = fwdRoad - podjedz_pan_start;
			}
		}
	if (parking_move == 5) {
		sidesignals = SIDETURN_LEFT;
		steering = -90.f;
		velocity = 500;
		if ((CumulativeYaw - start_angle) > (parking_turn_sharpness - unfinished_angle)
				|| (CumulativeYaw - start_angle) < -(parking_turn_sharpness - unfinished_angle)) {
			++parking_move;
			start_distance = fwdRoad;
			start_angle = CumulativeYaw;
		}
	}
	if (parking_move == 6) {
		sidesignals = SIDETURN_LEFT;
		steering = -(CumulativeYaw - start_angle);
		velocity = 500;
		if ((fwdRoad - start_distance) < parking_depth) {
			++parking_move;
			start_angle = CumulativeYaw;
		}
	}
	if (parking_move == 7) {
		sidesignals = SIDETURN_LEFT;
		steering = 90.f;
		velocity = 500;
		if ((CumulativeYaw - start_angle) > parking_turn_sharpness
				|| (CumulativeYaw - start_angle) < -parking_turn_sharpness) {

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
void lane_switch_f(void) {
	static int then = 0;
	if (flags[0] && lane_switching_move == 1) {
		then = HAL_GetTick();
		HAL_GPIO_WritePin(Change_Lane_GPIO_Port, Change_Lane_Pin,
				GPIO_PIN_RESET);
		sidesignals = SIDETURN_NONE;
		lane_switching_move = 2;
	} else if (!(flags[0]) && lane_switching_move == 2 && (HAL_GetTick() - then) > 70) {
		HAL_GPIO_WritePin(Change_Lane_GPIO_Port, Change_Lane_Pin, GPIO_PIN_SET);
		sidesignals = SIDETURN_RIGHT;
		then = HAL_GetTick();
		lane_switching_move = 3;
	} else if (lane_switching_move == 3 && (HAL_GetTick() - then) > 70) {

		hold_signals = 1;
		autonomous_task = lanefollower;
	}
	if (driving_state == autonomous)
		set_spd = 600;
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
		place_lenght = fwdRoad - start_place;
		if (place_lenght > 600) {
			start_place = fwdRoad;//poczatek podjazdu
			++parking_search_move;
		} else{
			parking_search_move = 1;
		}

	}
	if(parking_search_move ==3){//podjazd
		if(fwdRoad - start_place > parking_dead_fwd){
			autonomous_task = parking;
			parking_search_move=0;
		}

	}
}

void await_f(void)
 {
	set_spd = 0;
	if (HAL_GPIO_ReadPin(Parking_Button_GPIO_Port, Parking_Button_Pin) == GPIO_PIN_SET) {
		//ruszenie z boxu
	osDelay(500);
		challenge_select = 1;
/*
		set_spd = 500;
		TIM2->CCR3 = servo_middle;
		osDelay(2000);
		HAL_GPIO_TogglePin(Vision_Reset_GPIO_Port, Vision_Reset_Pin);
		osDelay(70);
		HAL_GPIO_TogglePin(Vision_Reset_GPIO_Port, Vision_Reset_Pin);
		*/
		autonomous_task = lanefollower;

    }
    else if(HAL_GPIO_ReadPin(Obstacle_Button_GPIO_Port,Obstacle_Button_Pin) == GPIO_PIN_SET){
		//ruszenie z boxu
    	osDelay(500);
    	challenge_select = 2;
/*
		set_spd = 500;
		TIM2->CCR3 = servo_middle;
		osDelay(2000);
		HAL_GPIO_TogglePin(Vision_Reset_GPIO_Port, Vision_Reset_Pin);
		osDelay(70);
		HAL_GPIO_TogglePin(Vision_Reset_GPIO_Port, Vision_Reset_Pin);
*/
		autonomous_task = lanefollower;
    }
}


void crossing_f(void) {
	//zatrzymanie

	if(crossing_move ==0){
		crossing_dist = fwdRoad;
		++crossing_move;
	}
	if (crossing_move == 1) {
		if (driving_state == autonomous)
			set_spd = 500;
		if (fwdRoad - crossing_dist > 150) {
			++crossing_move;
		}
	}
	if (crossing_move == 2) {
		set_spd = 0;

		HAL_GPIO_WritePin(Vision_Reset_GPIO_Port, Vision_Reset_Pin, GPIO_PIN_SET);
		osDelay(3000);
		HAL_GPIO_WritePin(Vision_Reset_GPIO_Port, Vision_Reset_Pin, GPIO_PIN_RESET);
		++crossing_move;
	}
	if(crossing_move ==3){
		crossing_dist = fwdRoad;
		++crossing_move;
	}
	//podjechanie metr do przodu
	if (crossing_move == 4) {
		if (driving_state == autonomous)
			set_spd = 700;
		if (fwdRoad - crossing_dist > 300) {
			crossing_move = 0;
			autonomous_task = lanefollower;
			HAL_GPIO_WritePin(Vision_Reset_GPIO_Port, Vision_Reset_Pin,
					GPIO_PIN_SET);
			osDelay(100);
			HAL_GPIO_WritePin(Vision_Reset_GPIO_Port, Vision_Reset_Pin,
					GPIO_PIN_RESET);
		} else if (fwdRoad - crossing_dist > 100) {
			HAL_GPIO_TogglePin(Cross_Obstacles_GPIO_Port, Cross_Obstacles_Pin);
			osDelay(100);
			HAL_GPIO_TogglePin(Cross_Obstacles_GPIO_Port, Cross_Obstacles_Pin);
		} else
			TIM2->CCR3 = servo_middle;
	}
}
void crossing_on_parking_f(void) {
	static uint8_t phase = 0;
	if (driving_state == autonomous)
		set_spd = 700;
	if (phase) {
		if (fwdRoad - crossing_dist > 450) {
			crossing_move = 0;
			autonomous_task = lanefollower;
			HAL_GPIO_WritePin(Vision_Reset_GPIO_Port, Vision_Reset_Pin,
					GPIO_PIN_SET);
			osDelay(100);
			HAL_GPIO_WritePin(Vision_Reset_GPIO_Port, Vision_Reset_Pin,
					GPIO_PIN_RESET);
			phase = 0;
		} else if (fwdRoad - crossing_dist > 250) {
			HAL_GPIO_TogglePin(Cross_Obstacles_GPIO_Port, Cross_Obstacles_Pin);
			osDelay(100);
			HAL_GPIO_TogglePin(Cross_Obstacles_GPIO_Port, Cross_Obstacles_Pin);
		} else
			TIM2->CCR3 = servo_middle;
	} else {
		crossing_dist = fwdRoad;
		phase = 1;
	}
}
void reset_all_to_challenge(void){

	sidesignals = SIDETURN_NONE;

	HAL_GPIO_WritePin(Obstacle_Button_GPIO_Port, Obstacle_Button_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Change_Lane_GPIO_Port, Change_Lane_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Vision_Reset_GPIO_Port, Vision_Reset_Pin, GPIO_PIN_RESET);
	hold_signals = 0;
	crossing_move = 0;
	parking_search_move = 0;
	parking_move = 0;
	lane_switching_move = 0;
	flags[0] = 0; flags[1] = 0; flags[2] = 0;

	if (challenge_select && autonomous_task != parkingsearch)
		autonomous_task = lanefollower;

	set_spd = 0;
	osDelay(800);
}


