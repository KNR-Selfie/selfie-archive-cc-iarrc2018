/*
 * Parking.c
 *
 *  Created on: 30.01.2018
 *      Author: User
 */
#include "main.h"
#include "stm32f7xx_hal.h"
#include "cmsis_os.h"
#include "Enc.h"
#include "BT.h"
#include "Gyro.h"
#include "Lighting.h"
#include "Czujniki.h"
#include "MotorControl.h"
#define SBUS_MAX_CHANNEL 18

extern uint16_t FutabaChannelData[SBUS_MAX_CHANNEL];
extern uint16_t range[8];
extern float set_spd;
extern uint8_t parking_mode;

//zmienne do wyszukania miejsca parkowania
uint8_t parking_status = 0;
float start_place;
float place_lenght;

uint8_t parking_mode = 0;
uint8_t parking_move = 0;
float parking_angle = 0;
float parking_speed = 0;
float start_angle = 0;
float start_distance = 0;

extern float set_spd;
extern float actualSpeed;
extern float pid_speed;

void StartParkingTask(void const * argument) {
	/*
	 0 oczekiwanie na przycisk
	 1 przycisk wciœniêty, szukaj pierwszego pude³ka
	 */
	while (1) {
		if (FutabaChannelData[5] == 1904) //zadanie z parkowaniem tylko w trybie autonomicznym
				{
			//oczekiwanie na przycisk
			if (HAL_GPIO_ReadPin(Parking_Button_GPIO_Port, Parking_Button_Pin)
					== GPIO_PIN_SET && parking_status == 0) {
				parking_status = 1;
				//dioda sygnalizujaca
				HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin,
						GPIO_PIN_RESET);
				set_spd = 500;
			}
			//wykrycie poczatku pudelka
			if (parking_status == 1 && range[6] < 200) {
				++parking_status;
			}
			//wykrycie konca pudelka - rozpoczecie mierzenia miejsca
			if (parking_status == 2 && range[6] > 200) {
				start_place = fwdRoad;
				++parking_status;
			}
			//wykrycie kolejnego pudelka
			if (parking_status == 3 && range[6] < 200) {
				++parking_status;
				place_lenght = fwdRoad - start_place;
			}
			if (parking_status == 4) {
				if (place_lenght > 200) {
					if (fwdRoad - start_place > (place_lenght + 100)) {
						parking_status = 5;
						parking_mode = 1;
					}
				} else
					parking_status = 1;
			}
		}
		if (parking_mode) {
					sidesignals = SIDETURN_RIGHT;
					float steering = 0, velocity = 0;
					if (parking_move == 0) {
						start_angle = CumulativeYaw;
						++parking_move;
						velocity = -100;
						parking_angle = steering;
						parking_speed = velocity;
						osSemaphoreRelease(DriveControlSemaphoreHandle);
						osDelay(100);
					}
					if (parking_move == 1) {
						++parking_move;
						velocity = 0;
						parking_angle = steering;
						parking_speed = velocity;
						osSemaphoreRelease(DriveControlSemaphoreHandle);
						osDelay(100);
					}
					if (parking_move == 2) {
						steering = -90.f;
						velocity = -500;
						if ((CumulativeYaw - start_angle) > 35
								|| (CumulativeYaw - start_angle) < -35){
							++parking_move;
							start_distance = fwdRoad;
							start_angle = CumulativeYaw;
						}
					}
					if (parking_move == 3) {

						steering = -(CumulativeYaw - start_angle);
						velocity = -500;
						if ((fwdRoad - start_distance) < -50)
						{
							++parking_move;
							start_angle = CumulativeYaw;
						}
					}
					if (parking_move == 4) {
						steering = 90.f;
						velocity = -500;
						if (range[0]<50) {
							steering = 0.f;
							velocity = 0;
							parking_move = 0;
							parking_mode = 0;
							parking_status=6;
						}
					}
					parking_angle = steering;
					parking_speed = velocity;
					osSemaphoreRelease(DriveControlSemaphoreHandle);
				}
				else
					sidesignals = SIDETURN_NONE;

		osDelay(10);
	}
}

