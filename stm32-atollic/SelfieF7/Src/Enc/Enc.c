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
#define kpServoPos 5.0
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

/************ Testy matiego **************/
#define M_PI_FLOAT  3.14159265358979323846f
#define ENC_RESOLUTION 10240
#define ENC_DT 0.005f
#define WHEEL_DIAMETER 50.f

//speeds
volatile int16_t vleft; //(mm/s)
volatile int16_t vright; //(mm/s)
volatile int16_t vfwd; //(mm/s)


float enc_coeff =  1.f/ (ENC_RESOLUTION / WHEEL_DIAMETER / M_PI_FLOAT * ENC_DT);

volatile int16_t leftCount;
volatile int16_t rightCount;

volatile int16_t fwdCount;
volatile int16_t rotCount;
//distances
volatile int32_t leftRoad; // mm
volatile int32_t rightRoad; // mm
volatile int32_t fwdRoad ; // mm

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
void encodersReset (void);
void encodersRead (void);
int16_t wheel_pid(float kp, float ki, float kd, int16_t setfwd);

/************ Testy matiego **************/

void StartEncTask(void const * argument) {
	MX_TIM5_Init();
	MX_TIM3_Init();
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start(&htim5, TIM_CHANNEL_ALL);
	encodersReset();

	pid_paramsinitEng(kpEng, kiEng, kdEng);
	pid_paramsinitServoPos(kpServoPos, kiServoPos, kdServoPos);
	pid_paramsinitServoAng(kpServoAng, kiServoAng, kdServoAng);

	float poczatek_kolo1 = 0;
	float poczatek_kolo2 = 0;
	float koniec_kolo1 = 0;
	float koniec_kolo2 = 0;

	while (1) {
		/*
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
		 */
		encodersRead();
		actualSpeed = ((leftCount)/0.005 + (rightCount)/0.005) * 0.5;
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
	float err_d, u;

	pid_paramsEngine.err = set_val - read_val;
	pid_paramsEngine.err_sum += pid_paramsEngine.err;

	if (pid_paramsEngine.err_sum > ERR_SUM_MAX_ENGINE) {
		pid_paramsEngine.err_sum = ERR_SUM_MAX_ENGINE;
	} else if (pid_paramsEngine.err_sum < -ERR_SUM_MAX_ENGINE) {
		pid_paramsEngine.err_sum = -ERR_SUM_MAX_ENGINE;
	}

	err_d = pid_paramsEngine.err - pid_paramsEngine.err_last;
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
	u = 1*uPos+uAng *0;

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
	leftEncoder = TIM3->CNT;
	oldRightEncoder = rightEncoder;
	rightEncoder = TIM5->CNT;
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
	vfwd = (vleft + vright) / 2 ;

	rightRoad +=vright*ENC_DT;
	leftRoad +=vleft*ENC_DT;
	fwdRoad +=vfwd*ENC_DT;
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
	int16_t pterm, iterm, dterm, pid;
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

	delta = error_last - error_sum;
	error_last = error_sum;

	pterm = kp * error;
	iterm = ki * error_sum;
	if (iterm > 250)
		iterm = 250;
	else if (iterm < -250)
		iterm = -250;

	dterm = kd * delta;

	pid = pterm + iterm + dterm;
	if (pid > 1000)
		pid = 1000;
	else if (pid < -1000)
		pid = 1000;

	return pid;
}
