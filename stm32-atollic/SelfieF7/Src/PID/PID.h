/*
 * PID.h
 *
 *  Created on: 21.01.2018
 *      Author: Michal
 */

#ifndef PID_PID_H_
#define PID_PID_H_

#include "cmsis_os.h"


#define kpEng 40
#define kiEng 0.3
#define kdEng 0.1
#define kpServoPos 1.55f
#define kiServoPos 0
#define kdServoPos 0.42f
#define kpServoAng 3.6
#define kiServoAng 0
#define kdServoAng 0

float pid_speed;
float pid_servo;


///////////////////////////////
//podgladanie bledu regulatorow
float errorE;
float wyjscieRegulatoraE;

float errorSP;
float errorSA;
float wyjscieRegulatoraSP;
float wyjscieRegulatoraSA;
//////////////////////////////
extern float set_spd;
extern float set_pos;
extern float set_angle;

//gdy running, sa liczone nowe wartosci PID
//gdy stopped nie sa liczone nowe (jest podawana stara wartosc) - err_sum niewyzerowane
//gdy reset nie sa liczone nowe (jest podawana stara wartosc) - err_sum wyzerowane
typedef enum {
	PID_STOPPED = 0,
    PID_RUNNING,
	PID_RESET,
} PidControllerState;

typedef struct pid_params
{
	float kp;
	float ki;
	float kd;
	float err;
	float err_sum;
	float err_last;
	PidControllerState state;
}pid_params;

pid_params pid_paramsEngine;
pid_params pid_paramsServoPos;
pid_params pid_paramsServoAng;

int16_t wheel_pid(float kp, float ki, float kd, int16_t setfwd);
void pid_paramsinit(pid_params *pid_param, float kp, float ki, float kd);
float pid_servo_f(void);
float pid_calculateEngine(pid_params *pid_param, float set_val, float read_val);
float pid_calculateServoPos(pid_params *pid_paramPos,float set_pos, float read_pos);
float pid_calculateServoAng(pid_params *pid_paramAng, float set_angle, float read_angle);
PidControllerState GetPidState(pid_params *pid_param);
void SetPidState(pid_params *pid_param, PidControllerState stat);
void ResetPidError(pid_params *pid_param);

void StartPID(void);

#endif /* PID_PID_H_ */
