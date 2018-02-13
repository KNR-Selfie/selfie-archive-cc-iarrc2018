/*
* Enc.c
*
*  Created on: 14.12.2017
*      Author: Michal
*/

#include "Steering.h"

#include "Governor.h"
#include "PID.h"
#include "Lighting.h"

#include "cmsis_os.h"
#include "math.h"
#include "tim.h"
#include "gpio.h"
#include "Filtering.h"


float actualSpeed = 0;

/************ Testy matiego **************/
#define M_PI_FLOAT  3.14159265358979323846f
#define ENC_RESOLUTION 10240
#define ENC_DT 0.005f
#define MOTOR_DT 0.02f
#define WHEEL_DIAMETER 64.f


volatile int16_t vleft; //(mm/s)
volatile int16_t vright; //(mm/s)
volatile int16_t vfwd; //(mm/s)


float enc_coeff = 1.f / (ENC_RESOLUTION / WHEEL_DIAMETER / M_PI_FLOAT * ENC_DT);
float enc_totalcoeff = 1.f/ (ENC_RESOLUTION / WHEEL_DIAMETER / M_PI_FLOAT);

volatile int16_t leftCount;
volatile int16_t rightCount;

volatile int16_t fwdCount;
volatile int16_t rotCount;
//distances
volatile float leftRoad; // mm
volatile float rightRoad; // mm
volatile float fwdRoad; // mm

volatile int32_t leftTotal;
volatile int32_t rightTotal;
volatile int32_t fwdTotal;
volatile int32_t rotTotal;

// local variables
static volatile int16_t oldLeftEncoder;
static volatile int16_t oldRightEncoder;
static volatile int16_t leftEncoder;
static volatile int16_t rightEncoder;
static volatile int16_t encoderSum;
static volatile int16_t encoderDiff;

float wheel_kp = 0.2f;
float wheel_ki = 0.004f;
float wheel_kd = 0.02f;

pt1Filter_t DtermLPF;

float set_spd = 0;
float set_pos = 0;
float set_angle = 0;
uint16_t dutyServo = 0;


extern float KpJetson;

uint8_t parking_mode;
float parking_angle;
float parking_speed;

uint16_t servo_middle = 1470;
uint16_t servo_bandwith = 300;

uint16_t AngleToServo(float angle);

/************ Testy matiego **************/

void StartSteeringTask(void const * argument) {
	MX_TIM5_Init();
	MX_TIM3_Init();
	TIM2->CCR4 = 5000;
	osDelay(100);
	TIM2->CCR4 = 1500;
	osDelay(500);
	//zabezpieczenie przed jazd¹ do ty³u
	TIM2->CCR4 = 1520;
	osDelay(50);

	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start(&htim5, TIM_CHANNEL_ALL);
	encodersReset();
	pt1FilterInit(&DtermLPF, 50, MOTOR_DT);

	StartPID();


	while (1) {
		encodersRead();
		vfwd;

		pid_speed = pid_calculateEngine(&pid_paramsEngine, set_spd, vfwd);
		TIM2->CCR4 = pid_speed;

		pid_servo = pid_servo_f();

		if(driving_state == fullcontrol)
			TIM2->CCR3 = dutyServo;
		else {
			if (autonomous_task == parking) {
				TIM2->CCR3 = AngleToServo(parking_angle);
			}

			else {
				TIM2->CCR3 = pid_servo;
			}
		}


		if ((set_spd > 0 && pid_speed < 1500) || set_spd < actualSpeed)
					brakesignals = BRAKE_NORMAL;
				else
					brakesignals = BRAKE_NONE;
		//static int16_t pid_value;
		/* motor loop needs own separate tick
		pid_value = wheel_pid(wheel_kp, wheel_ki, wheel_kd, set_spd);
		TIM2->CCR4 = 1500 + pid_value;
		osDelay(20);
		 */
		osDelay(5);
	}
}




void encodersRead(void) {
	oldLeftEncoder = leftEncoder;
	leftEncoder = TIM5->CNT;
	oldRightEncoder = rightEncoder;
	rightEncoder = TIM3->CNT;
	leftCount = leftEncoder - oldLeftEncoder;
	rightCount = rightEncoder - oldRightEncoder;
	fwdCount = leftCount + rightCount;
	rotCount = -(leftCount - rightCount);
	fwdTotal += fwdCount;
	rotTotal += rotCount;
	leftTotal += leftCount;
	rightTotal += rightCount;

	vleft = leftCount * enc_coeff;
	vright = rightCount * enc_coeff;
	vfwd = (vleft + vright) / 2;

	rightRoad = rightTotal * enc_totalcoeff;
	leftRoad = leftTotal * enc_totalcoeff;
	fwdRoad = 0.5f*leftRoad + 0.5f*rightRoad;
}
void encodersReset(void) {
	__disable_irq();
	oldLeftEncoder = 0;
	oldRightEncoder = 0;
	leftTotal = 0;
	rightTotal = 0;
	fwdTotal = 0;
	rotTotal = 0;

	vleft = 0;
	vright = 0;
	vfwd = 0;

	rightRoad = 0;
	leftRoad = 0;
	fwdRoad = 0;

	TIM3->CNT = 0;
	TIM5->CNT = 0;
	encodersRead();
	__enable_irq();
}
/* +/- 90^ */
uint16_t AngleToServo(float angle)
{
	return (servo_middle + (int16_t)(servo_bandwith * angle / 90.f));
}
