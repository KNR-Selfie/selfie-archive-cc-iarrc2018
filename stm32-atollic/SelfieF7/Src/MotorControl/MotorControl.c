/*
* MotorControl.c
*
*  Created on: 28.12.2017
*      Author: Michal
*/

#include "Lighting.h"
#include "Futaba.h"
#include "Czujniki.h"
#include "MotorControl.h"
#include "tim.h"

float set_spd = 0;
float set_pos = 0;
float set_angle = 0;
uint16_t dutyServo = 0;


extern float KpJetson;

extern uint8_t parking_mode;
extern float parking_angle;
extern float parking_speed;

uint16_t servo_middle = 943;

uint16_t AngleToServo(float angle);

void StartMotorControlTask(void const * argument) {
	//uruchomienie ESC
	TIM2->CCR4 = 2000;
	osDelay(1000);
	TIM2->CCR4 = 1500;
	osDelay(2000);

	//zabezpieczenie przed jazd¹ do ty³u
	TIM2->CCR4 = 1520;
	osDelay(50);


	while (1)
	{
		osSemaphoreWait(DriveControlSemaphoreHandle, osWaitForever);
		if (FutabaChannelData[5] < 500) //gorna pozycja przelacznika - pelna kontrola
		{
			if (((FutabaChannelData[1] - 1027) > 50) || ((FutabaChannelData[1] - 1027) < -50))
				set_spd = (1840 * (FutabaChannelData[1] - 1027) / (1680 - 368));
			else set_spd = 0;

			dutyServo = (servo_middle + 600 * (FutabaChannelData[3] - 1000) / (1921 - 80));
		}
		else if (FutabaChannelData[5] > 1500) //dolna pozycja prze31cznika, jazda autonomiczna
		{
			HAL_TIM_Base_Start_IT(&htim10); // timer od sprawdzania komunikacji
											//jezeli jest komunikacja na linii Jetson <-> STM
			if (parking_mode)
			{
				set_spd = parking_speed;
			}
			else if (j_syncByte == 255)
			{
					set_spd = 920;
					set_pos = 1000;
					set_angle = 90;
			}
			else if (j_syncByte == 200) //je?eli nie istnieje Jetson <-> STM, wylacz naped (wartosc j_syncByte = 200 jest ustawiana przez TIMER10)
			{
				set_spd = 0;
			}
		}
		else //srodkowa pozycja prze³acznika, tryb polautonomiczny
		{
			if (((FutabaChannelData[1] - 1027) > 50) || ((FutabaChannelData[1] - 1027) < -50))
				set_spd = (1840 * (FutabaChannelData[1] - 1027) / (1680 - 368));
			else set_spd = 0;
			set_pos = 1000;
			set_angle = 90;
		}
		if (FutabaChannelData[5] > 50)
			osSemaphoreRelease(EngineSemaphoreHandle);
	}
}
void StartDriveTask(void const * argument) {

	while (1) {
		osSemaphoreWait(EngineSemaphoreHandle, osWaitForever);
		if (FutabaChannelData[5] < 500)
			TIM2->CCR3 = dutyServo;
		else {
			if (parking_mode) {
				TIM2->CCR3 = AngleToServo(parking_angle);
			}
			else {
				TIM2->CCR3 = pid_servo;
			}
		}

		TIM2->CCR4 = pid_speed;

		if (pid_speed < 1500)
					brakesignals = BRAKE_NORMAL;
				else
					brakesignals = BRAKE_NONE;
	}
}

//void StartDriveTask(void const * argument){
//
//	while (1) {
//		osSemaphoreWait(EngineSemaphoreHandle, osWaitForever);
//		if (FutabaChannelData[5] < 500)
//			TIM2->CCR3 = dutyServo;
//		else
//		{
//			if (parking_mode) {
//				TIM2->CCR3 = AngleToServo(parking_angle);
//			} else {
//				float jetsonRatio = (j_jetsonData[0] - 1000.f) / 1000.f;
//				TIM2->CCR3 = 1400 - (int) (KpJetson * jetsonRatio);
////        			AngleToServo( (j_jetsonData[0] - 1000) * KpJetson /1000 );
////        	TIM2->CCR3 = pid_calculateServo(set_pos, set_angle, j_jetsonData[0], (j_jetsonData[1]+j_jetsonData[2])*0.5);
//			}
//		}
//		/* Dziala pid */
//		pid_speed = pid_calculateEngine(set_spd, actualSpeed);
//		TIM2->CCR4 = pid_speed; //(1500 + 1000 * (FutabaChannelData[1] - 1027) / (1680 - 368));
//
//	}
//}

/* +/- 90^ */
uint16_t AngleToServo(float angle)
{
	return (servo_middle + (int16_t)(300.f * angle / 90.f));
}
