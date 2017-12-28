/*
 * MotorControl.c
 *
 *  Created on: 28.12.2017
 *      Author: Michal
 */

#include "MotorControl.h"
#include "tim.h"
#include "cmsis_os.h"


float set_spd = 0;
float set_pos = 0;
float set_angle = 0;
uint16_t dutyServo = 0;

void StartMotorControlTask(void const * argument){
    //uruchomienie ESC
    TIM2->CCR4 = 2000;
    osDelay(1000);
    TIM2->CCR4 = 1500;
    osDelay(2000);

    //zabezpieczenie przed jazd¹ do ty³u
    TIM2->CCR4 = 1520;
    osDelay(50);

    int transition = 0;

    while (1)
        {
            osSemaphoreWait (DriveControlSemaphoreHandle, osWaitForever);
            if (a_channels[5] == 1024) //srodkowa pozycja prze³acznika, sterowanie z aparatury
                {
                    set_spd = (300000 * (a_channels[1] - 1027) / (1680 - 368));
                    dutyServo = (1400 + 800 * (a_channels[3] - 1000) / (1921 - 80));
                }
            else if (a_channels[5] == 144) //gorna pozycja prze31cznika, jazda autonomiczna
                {
                    HAL_TIM_Base_Start_IT (&htim10); // timer od sprawdzania komunikacji
                    //jezeli jest komunikacja na linii Jetson <-> STM
                    if (j_syncByte == 255)
                        {
                            //gdy brak wykrytej linii stop lub zbocze opadajace -> jedz.
                    	    //pozostawiona furtka zeby z BT zadawac spd/pos. Wystawic transition na 1 i dopisac kod na zadawanie z bt
                            if (!(transition))
                                {
                            	      set_spd = 30000;
                            	      set_pos = 1000;
                            	      set_angle = 90;
                                }
                        }
                    else if (j_syncByte == 200) //je?eli nie istnieje Jetson <-> STM, wylacz naped (wartosc j_syncByte = 200 jest ustawiana przez TIMER10)
                        {
                            set_spd = 0;
                        }
                }
            else if (a_channels[5] == 1904) //dolna pozycja prze³acznika, tryb pó³autonomiczny
                {
                    set_spd = (300000 * (a_channels[1] - 1027) / (1680 - 368));
                    set_pos = 1000;
                    set_angle = 90;
                }
            osSemaphoreRelease(EngineSemaphoreHandle);
        }
}

void StartDriveTask(void const * argument){

	while(1){
		osSemaphoreWait(EngineSemaphoreHandle, osWaitForever);
        if(a_channels[5] == 1024)
        	TIM2->CCR3 = dutyServo;
        else
        	TIM2->CCR3 = pid_calculateServo(set_pos, set_angle, j_jetsonData[0], (j_jetsonData[1]+j_jetsonData[2])*0.5);
        TIM2->CCR4 = pid_calculateEngine(set_spd, actualSpeed);
	}
}
