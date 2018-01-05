/*
 * BT.c
 *
 *  Created on: 19.12.2017
 *      Author: Michal
 */

#include "BT.h"
#include "cmsis_os.h"
#include "usart.h"

xSemaphoreHandle BTSemaphore = NULL;

////////testy BT
uint8_t choice = 0;
int tmp = 0;

uint16_t dane[4];
uint8_t data[32]; // Tablica przechowujaca wysylana wiadomosc.
int size;
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

void StartBTTask(void const * argument) {
	vSemaphoreCreateBinary(BTSemaphore);
	osSemaphoreWait(BTSemaphore, osWaitForever);
	MX_USART3_UART_Init();
	HAL_UART_Receive_DMA(&huart3, &choice, 1);
	while (1) {
		osSemaphoreWait(BTSemaphore, osWaitForever);
		size = sprintf((char*) data,"Otrzymano: %c\r\n", choice);
		HAL_UART_Transmit_DMA(&huart3, data, size);
		HAL_UART_Receive_DMA(&huart3, &choice, 1);
	}
}
void BluetoothRx_Irq(void) {
	osSemaphoreRelease(BTSemaphore);
}
void BluetoothTx_Irq(void) {
}
