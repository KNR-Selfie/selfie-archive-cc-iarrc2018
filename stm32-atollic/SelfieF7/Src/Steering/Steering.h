/*
 * Enc.h
 *
 *  Created on: 14.12.2017
 *      Author: Michal
 */

#ifndef STEERING_STEERING_H_
#define STEERING_STEERING_H_

#include "stm32f7xx_hal.h"
#include "Filtering.h"
#include "cmsis_os.h"

extern volatile int16_t vleft; //(mm/s)
extern volatile int16_t vright; //(mm/s)
extern volatile int16_t vfwd; //(mm/s)

extern volatile float leftRoad; // mm
extern volatile float rightRoad; // mm
extern volatile float fwdRoad ; // mm

extern pt1Filter_t DtermLPF;


void encodersRead(void);
void encodersReset(void);
void StartSteeringTask(void const * argument);

extern uint16_t j_jetsonData[8];
extern uint8_t j_jetsonFlags[2];
extern uint8_t j_syncByte;

extern float actualSpeed;
extern float pid_speed;
extern float pid_servo;

extern float parking_angle;
extern float parking_speed;

extern float set_spd;
extern float set_pos;
extern float set_angle;
extern uint16_t dutyServo;

/* SRODEK SERWA! */
uint16_t servo_middle;
uint16_t servo_bandwith;

#endif /* STEERING_STEERING_H_ */
