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
#include "Filtering.h"

#define ERR_SUM_MAX_ENGINE 300000
#define ERR_SUM_MAX_SERVOPOS 1500
#define ERR_SUM_MAX_SERVOANG 1500

#define kpEng 35
#define kiEng 0.2
#define kdEng 0.1
#define kpServoPos 3.5
#define kiServoPos 0
#define kdServoPos 0
#define kpServoAng 0 //3
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

/************ Testy matiego **************/
#define M_PI_FLOAT  3.14159265358979323846f
#define ENC_RESOLUTION 10240
#define ENC_DT 0.005f
#define MOTOR_DT 0.02f
#define WHEEL_DIAMETER 64.f

//speeds
extern float set_spd;

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
void encodersReset(void);
void encodersRead(void);

int16_t wheel_pid(float kp, float ki, float kd, int16_t setfwd);

/************ Testy matiego **************/

void StartEncTask(void const * argument) {
	MX_TIM5_Init();
	MX_TIM3_Init();
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start(&htim5, TIM_CHANNEL_ALL);
	encodersReset();
	pt1FilterInit(&DtermLPF, 50, MOTOR_DT);

	pid_paramsinitEng(kpEng, kiEng, kdEng);
	pid_paramsinitServoPos(kpServoPos, kiServoPos, kdServoPos);
	pid_paramsinitServoAng(kpServoAng, kiServoAng, kdServoAng);

	while (1) {
		encodersRead();
		actualSpeed = vfwd;

		/* motor loop needs own separate tick
		pid_value = wheel_pid(wheel_kp, wheel_ki, wheel_kd, set_spd);
		TIM2->CCR4 = 1500 + pid_value;
		osDelay(20);
		 */
		osDelay(5);
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
	read_val = read_val * 32.59485f;
	set_val = set_val * 32.59485f;

	float err_d, u;

	pid_paramsEngine.err = set_val - read_val;
	pid_paramsEngine.err_sum += pid_paramsEngine.err;

	if (pid_paramsEngine.err_sum > ERR_SUM_MAX_ENGINE) {
		pid_paramsEngine.err_sum = ERR_SUM_MAX_ENGINE;
	}
	else if (pid_paramsEngine.err_sum < -ERR_SUM_MAX_ENGINE) {
		pid_paramsEngine.err_sum = -ERR_SUM_MAX_ENGINE;
	}

	err_d = pid_paramsEngine.err - pid_paramsEngine.err_last;
	u = (pid_paramsEngine.kp * pid_paramsEngine.err + pid_paramsEngine.ki * pid_paramsEngine.err_sum
		+ pid_paramsEngine.kd * err_d);

	u = 1500 + (u / 5000);
	if (set_val < 0) u -= 140;
	else if (set_val > 0) u += 60;
	if (u > 1680) u = 1680;
	if (u< 1240) u = 1240;

	if (kierunek == 1 && set_val < 0) {
		TIM2->CCR4 = 1500;
		osDelay(25);
		TIM2->CCR4 = 1300;
		osDelay(25);
	}

	if (set_val > 0) kierunek = 1;
	else if (set_val < 0) kierunek = 2;
	else kierunek = 0;


	wyjscieRegulatoraE = u;
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
	}
	else if (pid_paramsServoPos.err_sum < -ERR_SUM_MAX_SERVOPOS) {
		pid_paramsServoPos.err_sum = -ERR_SUM_MAX_SERVOPOS;
	}

	err_d = pid_paramsServoPos.err - pid_paramsServoPos.err_last;
	uPos = (pid_paramsServoPos.kp * pid_paramsServoPos.err + pid_paramsServoPos.ki * pid_paramsServoPos.err_sum
		+ pid_paramsServoPos.kd * err_d);

	uPos = 943 + uPos;
	if (uPos > 1243) uPos = 1243;
	if (uPos < 643) uPos = 643;

	///////////////regulator od kata
	pid_paramsServoAng.err = set_angle - read_angle;
	pid_paramsServoAng.err_sum += pid_paramsServoAng.err;

	if (pid_paramsServoAng.err_sum > ERR_SUM_MAX_SERVOANG) {
		pid_paramsServoAng.err_sum = ERR_SUM_MAX_SERVOANG;
	}
	else if (pid_paramsServoAng.err_sum < -ERR_SUM_MAX_SERVOANG) {
		pid_paramsServoAng.err_sum = -ERR_SUM_MAX_SERVOANG;
	}

	err_d = pid_paramsServoAng.err - pid_paramsServoAng.err_last;
	uAng = (pid_paramsServoAng.kp * pid_paramsServoAng.err + pid_paramsServoAng.ki * pid_paramsServoAng.err_sum
		+ pid_paramsServoAng.kd * err_d);

//		uAng = 943 + uAng;
//		if(uAng > 1243) uAng = 1243;
//		if(uAng < 643) uAng = 643;


	/////////////waga dwoch wartosci z pida
	//u = (uPos*2+uAng)/3;
	u = uPos;

	////////dane do debuggowania
	errorS = pid_paramsServoPos.err + pid_paramsServoAng.err;
	errorSP = pid_paramsServoPos.err;
	errorSA = pid_paramsServoAng.err;
	wyjscieRegulatoraSP = uPos;
	wyjscieRegulatoraSA = uAng;
	wyjscieRegulatoraS = u;
	return u;
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
