/*
 * BT.c
 *
 *  Created on: 19.12.2017
 *      Author: Michal
 */

#include "BT.h"

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#include "main.h"
#include "usart.h"
#include "gpio.h"

/* Zeby miec podstawowe dane z Gyro i Baterii */
#include "Gyro.h"
#include "Enc.h"
#include "Lighting.h"
#include "Czujniki.h"
#include "MotorControl.h"
#include "Battery.h"
/* oraz silnika*/
extern float set_spd;
extern float actualSpeed;
extern float pid_speed;

float KpJetson = 3000.f;

extern osThreadId BTTaskHandle;
xSemaphoreHandle BTSemaphore = NULL;

////////testy BT
uint8_t choice = 0;
int tmp = 0;

uint16_t dane[4];
uint8_t txdata[256]; // Tablica przechowujaca wysylana wiadomosc.
uint8_t rxdata[32]; // Tablica przechowujaca odbierana wiadomosc.

uint32_t size;
uint8_t tekst[32];

int nr = 0;
uint8_t value[6];
int speed = 0;
float angle;
int flag = 0;
int flag2 = 0;
int sendflag = 0;

float r_kpPredkosc = 16.91;
float pom = 0;
uint8_t table[4];
char tab[6];
///////////KONIEC TESTY BT

uint8_t parking_mode = 0;
uint8_t parking_move = 0;
float parking_angle = 0;
float parking_speed = 0;
float start_angle = 0;
float start_distance = 0;

void BT_Commands(uint8_t* Buf, uint32_t length);


void StartBTTask(void const * argument) {
	vSemaphoreCreateBinary(BTSemaphore);
//	osSemaphoreWait(BTSemaphore, osWaitForever);
	MX_USART3_UART_Init();
	HAL_UART_Receive_DMA(&huart3, rxdata, 1);
	while (1) {
//		osSemaphoreWait(BTSemaphore, osWaitForever);
//		osSignalWait(0x01, osWaitForever);
		if (parking_mode) {
			sidesignals = SIDETURN_RIGHT;
			float steering = 0, velocity = 0;
			if (parking_move == 0) {
				start_angle = CumulativeYaw;
				++parking_move;
				velocity = -25000;
				parking_angle = steering;
				parking_speed = velocity;
				osSemaphoreRelease(DriveControlSemaphoreHandle);
				osDelay(20);
			}
			if (parking_move == 1) {
				++parking_move;
				velocity = 0;
				parking_angle = steering;
				parking_speed = velocity;
				osSemaphoreRelease(DriveControlSemaphoreHandle);
				osDelay(20);
			}
			if (parking_move == 2) {
				steering = -90.f;
				velocity = -25000;
				if ((CumulativeYaw - start_angle) > 35
						|| (CumulativeYaw - start_angle) < -35){
					++parking_move;
					start_distance = fwdRoad;
					start_angle = CumulativeYaw;
				}
			}
			if (parking_move == 3) {

				steering = -(CumulativeYaw - start_angle);
				velocity = -25000;
				if ((fwdRoad - start_distance) < -50)
				{
					++parking_move;
					start_angle = CumulativeYaw;
				}
			}
			if (parking_move == 4) {
				steering = 90.f;
				velocity = -25000;
				if ((CumulativeYaw - start_angle) > 35
						|| (CumulativeYaw - start_angle) < -35) {
					steering = 0.f;
					velocity = 0.f;
					parking_move = 0;
					parking_mode = 0;
					sidesignals = SIDETURN_NONE;
				}
			}
			parking_angle = steering;
			parking_speed = velocity;
			osSemaphoreRelease(DriveControlSemaphoreHandle);
		}
		osDelay(20);
	}
}
void BT_Commands(uint8_t* Buf, uint32_t length) {
	/* Very Simple */
	if (Buf[0] == 'a') {
		size = sprintf((char*) txdata,
				"YawRate\t\t= %.5f deg/s\r\nCumulativeYaw\t= %.5f deg\r\n\r\n",
				YawRate, CumulativeYaw);
		HAL_UART_Transmit_DMA(&huart3, txdata, size);
	} else if (Buf[0] == 'b') {
		size = sprintf((char*) txdata,
				"GyroTemp\t= %.2f degC\r\n\r\n",
				temperature);
		HAL_UART_Transmit_DMA(&huart3, txdata, size);
	} else if (Buf[0] == 'c') {
		size = sprintf((char*) txdata,
				"Voltage\t\t= %.2f Volts\r\nCurrent\t= %.2f Amps\r\nFuel\t= %.1f mAh\r\n\r\n",
				Volts_f, Amps_f, mAhs_drawn);
		HAL_UART_Transmit_DMA(&huart3, txdata, size);
	} else if (Buf[0] == 'e') {
		size = sprintf((char*) txdata,
				"vleft = %d \r\nvright = %d \r\nvfwd = %d \r\ndistleft = %.1f \r\ndistright = %.1f\r\nfwdDist = %.1f \r\n\r\n",
				vleft, vright, vfwd, leftRoad, rightRoad, fwdRoad);
		HAL_UART_Transmit_DMA(&huart3, txdata, size);
	} else if (Buf[0] == 'v') {
		size = sprintf((char*) txdata, "vlx distance\t\t= %d\r\n\r\n", range);
		HAL_UART_Transmit_DMA(&huart3, txdata, size);
	} else if (Buf[0] == 'p') {
		if (parking_mode)
			parking_mode = 0;
		else {
			parking_mode = 1;
			size = sprintf((char*) txdata, "bede parkowal \r\n\r\n");
			HAL_UART_Transmit_DMA(&huart3, txdata, size);
		}
	} else if (Buf[0] == '+') {
		KpJetson += 20.f;
		size = sprintf((char*) txdata, "Kp = %.1f \r\n\r\n", KpJetson);
		HAL_UART_Transmit_DMA(&huart3, txdata, size);
	} else if (Buf[0] == '-') {
		KpJetson -= 20.f;
		size = sprintf((char*) txdata,"Kp = %.1f \r\n\r\n", KpJetson);
		HAL_UART_Transmit_DMA(&huart3, txdata, size);
	} else if (Buf[0] == 'R')
		NVIC_SystemReset();
}
void BluetoothRx_Irq(void) {
//	osSemaphoreRelease(BTSemaphore);
//	osSignalSet(BTTaskHandle, 0x01);
	BT_Commands(rxdata, 1);
	HAL_UART_Receive_DMA(&huart3, rxdata, 1);
}
void BluetoothTx_Irq(void) {
}
