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
#include "Lighting.h"
#include "Czujniki.h"
#include "Governor.h"
#include "Battery.h"
#include "Steering.h"

//informacja o flagach z przerwania odroida
extern int ParkingFlag;
extern int CrossFlag;

uint8_t LastCrossFlag = 0;
uint8_t LastParkingFlag = 0;

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


void BT_Commands(uint8_t* Buf, uint32_t length);


void StartBTTask(void const * argument) {
	vSemaphoreCreateBinary(BTSemaphore);
//	osSemaphoreWait(BTSemaphore, osWaitForever);
	MX_USART3_UART_Init();
	HAL_UART_Receive_DMA(&huart3, rxdata, 1);
	while (1) {
		osSemaphoreWait(BTSemaphore, osWaitForever);
//		osSignalWait(0x01, osWaitForever);
//		osDelay(20);
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
		size = 0;
		for (int sensor = 0; sensor < VLX_SENSOR_COUNT; sensor++)
			size += sprintf((char*) txdata + size, "vlx[%d] = %d\r\n",
					sensor+1, range[sensor]);
		size += sprintf((char*)txdata + size, "\r\n");
		HAL_UART_Transmit_DMA(&huart3, txdata, size);
	}else if (Buf[0] == 's') {
		size = 0;
		for (int sensor = 0; sensor < 3; sensor++)
			size += sprintf((char*) txdata + size, "flags[%d] = %d\r\n",
					sensor+1, flags[sensor]);
		size += sprintf((char*)txdata + size, "\r\n");
		HAL_UART_Transmit_DMA(&huart3, txdata, size);
	} else if (Buf[0] == 'p') {
		if (autonomous_task == parking)
			autonomous_task = lanefollower;
		else {
			autonomous_task = parkingsearch;
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
	} else if (Buf[0] == '1') {
		servo_middle++;
		size = sprintf((char*) txdata, "servo_middle = %d \r\n\r\n", servo_middle);
		HAL_UART_Transmit_DMA(&huart3, txdata, size);
	} else if (Buf[0] == '2') {
		servo_middle--;
		size = sprintf((char*) txdata, "servo_middle = %d \r\n\r\n", servo_middle);
		HAL_UART_Transmit_DMA(&huart3, txdata, size);
	} else if (Buf[0] == '9') {
		lane_change_treshold += 10;
		size = sprintf((char*) txdata, "lane treshold = %dmm \r\n\r\n", lane_change_treshold);
		HAL_UART_Transmit_DMA(&huart3, txdata, size);
	} else if (Buf[0] == '0') {
		lane_change_treshold -= 10;
		size = sprintf((char*) txdata, "lane treshold = %dmm \r\n\r\n", lane_change_treshold);
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
