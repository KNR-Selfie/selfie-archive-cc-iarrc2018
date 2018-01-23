/*
 * PID.h
 *
 *  Created on: 21.01.2018
 *      Author: Michal
 */

#ifndef PID_PID_H_
#define PID_PID_H_

#include "cmsis_os.h"

extern float actualSpeed;
extern float set_spd;
extern float set_pos;
extern float set_angle;
extern osSemaphoreId PIDSemaphoreHandle;

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

int16_t wheel_pid(float kp, float ki, float kd, int16_t setfwd);
void pid_paramsinit(pid_params *pid_param, float kp, float ki, float kd);
float pid_calculateEngine(pid_params *pid_param, float set_val, float read_val);
float pid_calculateServo(pid_params *pid_paramPos, pid_params *pid_paramAng, float set_pos, float set_angle, float read_pos, float read_angle);
PidControllerState GetPidState(pid_params *pid_param);
void SetPidState(pid_params *pid_param, PidControllerState stat);

void StartPIDTask(void const * argument);

#endif /* PID_PID_H_ */
