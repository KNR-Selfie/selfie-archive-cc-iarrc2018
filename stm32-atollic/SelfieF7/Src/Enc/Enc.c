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

#define ERR_SUM_MAX_ENGINE 1000000
#define ERR_SUM_MAX_SERVOPOS 1500
#define ERR_SUM_MAX_SERVOANG 1500

#define kpEng 6.32379796804752
#define kiEng 42.982508097399*0.005
#define kdEng 0
#define kpServoPos 2.5
#define kiServoPos 0
#define kdServoPos 0
#define kpServoAng 10
#define kiServoAng 0
#define kdServoAng 0


///////////////////////////////
//podgladanie bledu regulatorow
float errorE = 0;
float wyjscieRegulatoraE = 0;
float actualSpeed = 0;

float errorS = 0;
float errorSP = 0;
float errorSA = 0;
float wyjscieRegulatoraS = 0;
float wyjscieRegulatoraSP = 0;
float wyjscieRegulatoraSA = 0;
//////////////////////////////

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
static struct pid_params pid_paramsServoPos;
static struct pid_params pid_paramsServoAng;


void pid_paramsinitEng(float kp, float ki, float kd);
void pid_paramsinitServoPos(float kp, float ki, float kd);
void pid_paramsinitServoAng(float kp, float ki, float kd);
float pid_calculateEngine(float set_val, float read_val);
float pid_calculateServo(float set_pos, float set_angle, float read_pos, float read_angle);


void StartEncTask(void const * argument) {
	pid_paramsinitEng(kpEng, kiEng, kdEng);
	pid_paramsinitServoPos(kpServoPos, kiServoPos, kdServoPos);
	pid_paramsinitServoAng(kpServoAng, kiServoAng, kdServoAng);

	float poczatek_kolo1 = 0;
	float poczatek_kolo2 = 0;
	float koniec_kolo1 = 0;
	float koniec_kolo2 = 0;

	while (1) {
		TIM3->CNT = 0;
	  	poczatek_kolo1 = TIM3->CNT;
	  	poczatek_kolo2 = TIM3->CNT;
	  	osDelay(5);
	  	  koniec_kolo1 = TIM3->CNT;
	  	  koniec_kolo2 = TIM3->CNT;
			if(koniec_kolo1 > 60000)
					koniec_kolo1 -= 65535;
			if(koniec_kolo2 > 60000)
					koniec_kolo2 -= 65535;
		actualSpeed = ((koniec_kolo1 - poczatek_kolo1)/0.005 + (koniec_kolo2 - poczatek_kolo2)/0.005) * 0.5;
	}
}


void pid_paramsinitEng(float kp, float ki, float kd)
{
	pid_paramsEngine.kp = kp;
	pid_paramsEngine.ki = ki;
	pid_paramsEngine.kd = kd;
	pid_paramsEngine.err = 0;
	pid_paramsEngine.err_sum = 0;
	pid_paramsEngine.err_last = 0;
}

void pid_paramsinitServoPos(float kp, float ki, float kd)
{
	pid_paramsServoPos.kp = kp;
	pid_paramsServoPos.ki = ki;
	pid_paramsServoPos.kd = kd;
	pid_paramsServoPos.err = 0;
	pid_paramsServoPos.err_sum = 0;
	pid_paramsServoPos.err_last = 0;
}

void pid_paramsinitServoAng(float kp, float ki, float kd)
{
	pid_paramsServoAng.kp = kp;
	pid_paramsServoAng.ki = ki;
	pid_paramsServoAng.kd = kd;
	pid_paramsServoAng.err = 0;
	pid_paramsServoAng.err_sum = 0;
	pid_paramsServoAng.err_last = 0;
}


//zamiana na static wewnatrz pid_calculateengine
uint8_t kierunek = 0; //0 stoi, 1 przod, 2 tyl
float pid_calculateEngine(float set_val, float read_val)
{
	float err_d, u;

	pid_paramsEngine.err = set_val - read_val;
	pid_paramsEngine.err_sum += pid_paramsEngine.err;

	if (pid_paramsEngine.err_sum > ERR_SUM_MAX_ENGINE) {
		pid_paramsEngine.err_sum = ERR_SUM_MAX_ENGINE;
	} else if (pid_paramsEngine.err_sum < -ERR_SUM_MAX_ENGINE) {
		pid_paramsEngine.err_sum = -ERR_SUM_MAX_ENGINE;
	}

	err_d = pid_paramsEngine.err_last - pid_paramsEngine.err;
	u = (pid_paramsEngine.kp * pid_paramsEngine.err + pid_paramsEngine.ki * pid_paramsEngine.err_sum
			+ pid_paramsEngine.kd * err_d);

	wyjscieRegulatoraE = u;
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


	errorE = pid_paramsEngine.err;
	return u;
}



float pid_calculateServo(float set_pos, float set_angle, float read_pos, float read_angle)
{
	float err_d, u, uPos, uAng;

	//////////////regulator od pozycji
	pid_paramsServoPos.err = set_pos - read_pos;
	pid_paramsServoPos.err_sum += pid_paramsServoPos.err;

	if (pid_paramsServoPos.err_sum > ERR_SUM_MAX_SERVOPOS) {
		pid_paramsServoPos.err_sum = ERR_SUM_MAX_SERVOPOS;
	} else if (pid_paramsServoPos.err_sum < -ERR_SUM_MAX_SERVOPOS) {
		pid_paramsServoPos.err_sum = -ERR_SUM_MAX_SERVOPOS;
	}

	err_d = pid_paramsServoPos.err_last - pid_paramsServoPos.err;
	uPos = (pid_paramsServoPos.kp * pid_paramsServoPos.err + pid_paramsServoPos.ki * pid_paramsServoPos.err_sum
			+ pid_paramsServoPos.kd * err_d);

	uPos = 1400 + uPos;
	if(uPos > 1700) uPos = 1700;
	if(uPos < 1100) uPos = 1100;

	///////////////regulator od kata
	pid_paramsServoAng.err = set_angle - read_angle;
	pid_paramsServoAng.err_sum += pid_paramsServoAng.err;

	if (pid_paramsServoAng.err_sum > ERR_SUM_MAX_SERVOANG) {
		pid_paramsServoAng.err_sum = ERR_SUM_MAX_SERVOANG;
	} else if (pid_paramsServoAng.err_sum < -ERR_SUM_MAX_SERVOANG) {
		pid_paramsServoAng.err_sum = -ERR_SUM_MAX_SERVOANG;
	}

	err_d = pid_paramsServoAng.err_last - pid_paramsServoAng.err;
	uAng = (pid_paramsServoAng.kp * pid_paramsServoAng.err + pid_paramsServoAng.ki * pid_paramsServoAng.err_sum
			+ pid_paramsServoAng.kd * err_d);

	uAng = 1400 - uAng;
	if(uAng > 1700) uPos = 1700;
	if(uAng < 1100) uPos = 1100;


	/////////////waga dwoch wartosci z pida
	u = (uPos+uAng)*0.5;

	////////dane do debuggowania
	errorS = pid_paramsServoPos.err + pid_paramsServoAng.err;
	errorSP = pid_paramsServoPos.err;
	errorSA = pid_paramsServoAng.err;
	wyjscieRegulatoraSP = uPos;
	wyjscieRegulatoraSA = uAng;
	wyjscieRegulatoraS = u;
	return u;
}
