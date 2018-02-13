/*
 * PID.c
 *
 *  Created on: 21.01.2018
 *      Author: Michal
 */


#include "PID.h"
#include "cmsis_os.h"

#include "Governor.h"
#include "tim.h"
#include "Filtering.h"

#include "Steering.h"

#define ERR_SUM_MAX_ENGINE 300000
#define ERR_SUM_MAX_SERVOPOS 1500
#define ERR_SUM_MAX_SERVOANG 1500

#define kpEng 40
#define kiEng 0.3
#define kdEng 0.1
#define kpServoPos 1.2
#define kiServoPos 0
#define kdServoPos 0
#define kpServoAng 3
#define kiServoAng 0
#define kdServoAng 0


void pid_paramsinit(pid_params *pid_param, float kp, float ki, float kd){
	pid_param->kp = kp;
	pid_param->ki = ki;
	pid_param->kd = kd;
	pid_param->err = 0;
	pid_param->err_sum = 0;
	pid_param->err_last = 0;
	pid_param->state = PID_RESET;
}


float pid_calculateEngine(pid_params *pid_param, float set_val, float read_val)
{
	read_val = read_val * 32.59485f;
	set_val = set_val * 32.59485f;

	float err_d, u;
	static float u_last = 0;

	pid_param->err = set_val - read_val;
	pid_param->err_sum += pid_param->err;

	if (pid_param->err_sum > ERR_SUM_MAX_ENGINE) {
		pid_param->err_sum = ERR_SUM_MAX_ENGINE;
	}
	else if (pid_param->err_sum < -ERR_SUM_MAX_ENGINE) {
		pid_param->err_sum = -ERR_SUM_MAX_ENGINE;
	}

	err_d = pid_param->err - pid_param->err_last;
	u = (pid_param->kp * pid_param->err + pid_param->ki * pid_param->err_sum
		+ pid_param->kd * err_d);

	u = 1500 + (u / 5000);
	if (set_val < 0) u -= 140;
	else if (set_val > 0) u += 60;
	if (u > 2000) u = 2000;
	if (u< 1000) u = 1000;


	//ponizej jazda do tylu od razu po zadaniu ujemnej predkosci - bez koniecznosci "wracania" palcem na futabie
	//wyglada kretynsko, ale dziala
	static int transition = 0;
	if (u > 1490)
		transition = 0;

	if (set_val < 0) {
		if (transition < 10) {
			transition++;
			u = 1300;
		} else if (transition > 9 && transition < 30) {
			transition++;
			u = 1488;
		} else if (transition >= 30)
		{
			u = (pid_param->kp * pid_param->err + pid_param->ki * pid_param->err_sum
					+ pid_param->kd * err_d);

				u = 1500 + (u / 5000);
				if (set_val < 0) u -= 140;
				else if (set_val > 0) u += 60;
				if (u > 2000) u = 2000;
				if (u< 1000) u = 1000;
		}
	}
	else transition = 0;

	if(pid_param->state != PID_RUNNING) u = u_last;
	if(pid_param->state == PID_RESET) pid_param->err_sum = 0;
	wyjscieRegulatoraE = u;
	errorE = pid_param->err;
	u_last = u;
	return u;
}



float pid_calculateServoPos(pid_params *pid_paramPos, float set_pos, float read_pos)
{
	float err_d, u, uPos;
	static float uPos_last = 0;

	//////////////regulator od pozycji
	//if(((j_jetsonData[1] + j_jetsonData[2]) * 0.5) < 90)
		//read_pos = read_pos + (80 - (j_jetsonData[1] + j_jetsonData[2] * 0.5));
	//else if(((j_jetsonData[1] + j_jetsonData[2]) * 0.5) > 90)
		//read_pos = read_pos - ((j_jetsonData[1] + j_jetsonData[2] * 0.5) - 90);

	pid_paramPos->err = set_pos - read_pos;
	pid_paramPos->err_sum += pid_paramPos->err;

	if (pid_paramPos->err_sum > ERR_SUM_MAX_SERVOPOS) {
		pid_paramPos->err_sum = ERR_SUM_MAX_SERVOPOS;
	}
	else if (pid_paramPos->err_sum < -ERR_SUM_MAX_SERVOPOS) {
		pid_paramPos->err_sum = -ERR_SUM_MAX_SERVOPOS;
	}

	err_d = pid_paramPos->err - pid_paramPos->err_last;
	uPos = (pid_paramPos->kp * pid_paramPos->err + pid_paramPos->ki * pid_paramPos->err_sum
		+ pid_paramPos->kd * err_d);


	uPos = servo_middle - uPos;
	if (uPos > (servo_middle+servo_bandwith)) uPos = servo_middle+servo_bandwith;
	if (uPos < (servo_middle-servo_bandwith)) uPos = servo_middle-servo_bandwith;



	if(pid_paramPos->state != PID_RUNNING) uPos = uPos_last;
	if(pid_paramPos->state == PID_RESET) pid_paramPos->err_sum = 0;

	u = uPos;


	errorSP = pid_paramPos->err;
	wyjscieRegulatoraSP = uPos;
	uPos_last = uPos;
	return u;
}


float pid_calculateServoAng(pid_params *pid_paramAng, float set_angle, float read_angle)
{
	float err_d, u, uAng;
	static float uAng_last = 0;

	///////////////regulator od kata
	pid_paramAng->err = set_angle - read_angle;
	pid_paramAng->err_sum += pid_paramAng->err;

	if (pid_paramAng->err_sum > ERR_SUM_MAX_SERVOANG) {
		pid_paramAng->err_sum = ERR_SUM_MAX_SERVOANG;
	}
	else if (pid_paramAng->err_sum < -ERR_SUM_MAX_SERVOANG) {
		pid_paramAng->err_sum = -ERR_SUM_MAX_SERVOANG;
	}

	err_d = pid_paramAng->err - pid_paramAng->err_last;
	uAng = (pid_paramAng->kp * pid_paramAng->err + pid_paramAng->ki * pid_paramAng->err_sum
		+ pid_paramAng->kd * err_d);

	//	uAng = servo_middle + uAng;
	//	if(uAng > (servo_middle+servo_bandwith)) uAng = servo_middle+servo_bandwith;
	//	if(uAng < (servo_middle-servo_bandwith)) uAng = servo_middle-servo_bandwith;


	if(pid_paramAng->state != PID_RUNNING) uAng = uAng_last;
	if(pid_paramAng->state == PID_RESET) pid_paramAng->err_sum = 0;


	u = uAng;

	////////dane do debuggowania
	errorSA = pid_paramAng->err;
	wyjscieRegulatoraSA = uAng;
	uAng_last = uAng;
	return u;
}

PidControllerState GetPidState(pid_params *pid_param){
	return pid_param->state;
}

void SetPidState(pid_params *pid_param, PidControllerState stat){
	pid_param->state = stat;
}

void ResetPidError(pid_params *pid_param){
	pid_param->err_sum = 0;
}

int16_t wheel_pid(float kp, float ki, float kd, int16_t setfwd) {
	int16_t pid;
	float pterm, iterm, dterm;
	int32_t error, delta;
	static int32_t error_sum;
	static int32_t error_last;


	encodersRead();
	error = setfwd - vfwd;
	error_sum += error;
	if (error_sum > 10000)
		error_sum = 10000;
	else if (error_sum < -10000)
		error_sum = -10000;


	delta = pt1FilterApply(&DtermLPF, (float)(error_last - error));
	error_last = error;

	pterm = kp * error;
	iterm = ki * error_sum;
	if (iterm > 125)
		iterm = 125;
	else if (iterm < -125)
		iterm = -125;

	dterm = kd * delta;

	pid = (int16_t)(pterm + iterm + dterm);
	if (pid > 500)
		pid = 500;
	else if (pid < -500)
		pid = -500;

	return pid;
}

void StartPID(void){

	pid_speed = 0;
	pid_servo = 0;
	errorE = 0;
	wyjscieRegulatoraE = 0;
	errorSP = 0;
	errorSA = 0;
	wyjscieRegulatoraSP = 0;
	wyjscieRegulatoraSA = 0;

	pid_paramsinit(&pid_paramsEngine, kpEng, kiEng, kdEng);
	pid_paramsinit(&pid_paramsServoPos, kpServoPos, kiServoPos, kdServoPos);
	pid_paramsinit(&pid_paramsServoAng, kpServoAng, kiServoAng, kdServoAng);

	SetPidState(&pid_paramsEngine, PID_RUNNING);
	SetPidState(&pid_paramsServoPos, PID_RUNNING);
	SetPidState(&pid_paramsServoAng, PID_STOPPED);
}
float pid_servo_f(void)
{
	float pid_servoPos;
	float pid_servoAng;

	pid_servoPos = pid_calculateServoPos(&pid_paramsServoPos, set_pos,
			j_jetsonData[0]);
	pid_servoAng = pid_calculateServoAng(&pid_paramsServoAng, set_angle,
			(j_jetsonData[1] + j_jetsonData[2]) * 0.5);

	if ((GetPidState(&pid_paramsServoAng) == PID_RUNNING)
			&& (GetPidState(&pid_paramsServoPos) == PID_RUNNING))
		return (pid_servoPos * 2 + pid_servoAng) / 3;

	else if ((GetPidState(&pid_paramsServoAng) == PID_STOPPED)
			&& (GetPidState(&pid_paramsServoPos) == PID_RUNNING))
		return pid_servoPos;
	else
		return pid_servoPos;
}
