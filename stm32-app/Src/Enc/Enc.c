/*
 * Enc.c
 *
 *  Created on: 14.12.2017
 *      Author: Michal
 */

#include "Enc.h"

#include "cmsis_os.h"
#include "math.h"
#include "tim.h"
#include "gpio.h"

#define ERR_SUM_MAX		1000000
#define ERR_SUM_MAX_SERVO 1500

#define r_kp 6.32379796804752
#define r_ki 42.982508097399*0.005
#define r_kd 0//-0.0329815060725376

#define r_kpServo 2.5
float KPG = 2;
#define r_kiServo 0
#define r_kdServo 0//-0.0329815060725376

//zmienne do podgladania
volatile uint16_t pulse_count; // Licznik impulsow
volatile uint16_t pulse_count2; // Licznik impulsow


float error = 0;
float wyjscieRegulatora = 0;
float actualSpeed = 0;
float poczatek = 0;
float koniec = 0;

float errorS = 0;
float wyjscieRegulatoraS = 0;

struct pid_params
{
	float kp;
	float ki;
	float kd;
	float err;
	float err_sum;
	float err_last;
};

static struct pid_params pid_paramsEngine;
static struct pid_params pid_paramsServo;


void pid_initEngine(float kp, float ki, float kd);
void pid_initServo(float kp, float ki, float kd);
float pid_calculateEngine(float set_val, float read_val);
float pid_calculateServo(float set_val, float read_val);


//volatile float wyjscie[500];
//int i;
void StartEncTask(void const * argument) {
	//osDelay(5000);
	pid_initEngine(r_kp, r_ki, r_kd);
	pid_initServo(r_kpServo, r_kiServo, r_kdServo);

//	for(i = 0; i<500; i++) wyjscie[i] = 0;
//	i = 0;
	while (1) {
		//TIM2->CCR4 = 1700;
		TIM3->CNT = 0;
	  	poczatek = TIM3->CNT;
	  	osDelay(5);
	  	  koniec = TIM3->CNT;
		if(koniec > 60000) {
				koniec -= 65535;

			}
		actualSpeed = (koniec - poczatek)/0.005;
//		if(i<500)
//		wyjscie[i++] = actualSpeed;

		pulse_count = TIM3->CNT;
		pulse_count2 = TIM5->CNT;
	}
}


void pid_initEngine(float kp, float ki, float kd)
{
	pid_paramsEngine.kp = kp;
	pid_paramsEngine.ki = ki;
	pid_paramsEngine.kd = kd;
	pid_paramsEngine.err = 0;
	pid_paramsEngine.err_sum = 0;
	pid_paramsEngine.err_last = 0;
}

void pid_initServo(float kp, float ki, float kd)
{
	pid_paramsServo.kp = kp;
	pid_paramsServo.ki = ki;
	pid_paramsServo.kd = kd;
	pid_paramsServo.err = 0;
	pid_paramsServo.err_sum = 0;
	pid_paramsServo.err_last = 0;
}

uint8_t kierunek = 0; //0 stoi, 1 przod, 2 tyl
float pid_calculateEngine(float set_val, float read_val)
{
	float err_d, u;

	pid_paramsEngine.err = set_val - read_val;
	pid_paramsEngine.err_sum += pid_paramsEngine.err;

	if (pid_paramsEngine.err_sum > ERR_SUM_MAX) {
		pid_paramsEngine.err_sum = ERR_SUM_MAX;
	} else if (pid_paramsEngine.err_sum < -ERR_SUM_MAX) {
		pid_paramsEngine.err_sum = -ERR_SUM_MAX;
	}

	err_d = pid_paramsEngine.err_last - pid_paramsEngine.err;
	u = (pid_paramsEngine.kp * pid_paramsEngine.err + pid_paramsEngine.ki * pid_paramsEngine.err_sum
			+ pid_paramsEngine.kd * err_d);

	u = 1500 + (u/5000);
	if(set_val < 0) u -= 100;
	else if(set_val > 0) u += 60;
	if(u > 1700) u = 1700;
	if(u< 1300) u = 1300;

	if(kierunek == 1 && set_val < 0){
	TIM2->CCR4 = 1300;
	osDelay(25);
	TIM2->CCR4 = 1500;
	osDelay(25);
	}

	if(set_val > 0) kierunek = 1;
	else if(set_val < 0) kierunek = 2;
	else kierunek =0;


	error = pid_paramsEngine.err;
	wyjscieRegulatora = u;
	return u;
}


float pid_calculateServo(float set_val, float read_val)
{
	float err_d, u;

	pid_paramsServo.err = set_val - read_val;
	pid_paramsServo.err_sum += pid_paramsServo.err;

	if (pid_paramsServo.err_sum > ERR_SUM_MAX) {
		pid_paramsServo.err_sum = ERR_SUM_MAX;
	} else if (pid_paramsServo.err_sum < -ERR_SUM_MAX) {
		pid_paramsServo.err_sum = -ERR_SUM_MAX;
	}

	err_d = pid_paramsServo.err_last - pid_paramsServo.err;
	u = (pid_paramsServo.kp * pid_paramsServo.err + pid_paramsServo.ki * pid_paramsServo.err_sum
			+ pid_paramsServo.kd * err_d);

	u = 1400 + u;
	if(u > 1700) u = 1700;
	if(u< 1100) u = 1100;


	errorS = pid_paramsServo.err;
	wyjscieRegulatoraS = u;
	return u;
}
