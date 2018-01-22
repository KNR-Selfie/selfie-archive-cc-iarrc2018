/*
 * MotorControl.h
 *
 *  Created on: 28.12.2017
 *      Author: Michal
 */

#ifndef MOTORCONTROL_MOTORCONTROL_H_
#define MOTORCONTROL_MOTORCONTROL_H_

#include "cmsis_os.h"
#include "PID.h"

extern uint16_t j_jetsonData[8];
extern uint8_t j_jetsonFlags[2];
extern uint8_t j_syncByte;



extern float actualSpeed;
extern float pid_speed;
extern float pid_servo;

extern osSemaphoreId DriveControlSemaphoreHandle;
extern osSemaphoreId EngineSemaphoreHandle;

/* SRODEK SERWA! */

extern uint16_t servo_middle;

extern float pid_calculateEngine(pid_params *pid_param, float set_val, float read_val);
extern float pid_calculateServo(pid_params *pid_paramPos, pid_params *pid_paramAng, float set_pos, float set_angle, float read_pos, float read_angle);


void StartDriveTask(void const * argument);
void StartMotorControlTask(void const * argument);


#endif /* MOTORCONTROL_MOTORCONTROL_H_ */
