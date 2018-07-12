/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */     
#include "Lighting.h"
#include "Gyro.h"
#include "Battery.h"
#include "Czujniki.h"
#include "BT.h"
#include "main.h"
#include "gpio.h"

#include "usbd_cdc_if.h"

#include "Steering.h"
extern USBD_HandleTypeDef hUsbDeviceFS;
uint8_t usbTxBuffer[256];
uint32_t len;
uint8_t able_to_receive = 0;

extern float set_spd;
extern float actualSpeed;
extern float pid_speed;
/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId defaultTaskHandle;
osThreadId LightingTaskHandle;
osThreadId GyroTaskHandle;
osThreadId BatteryManagerHandle;
osThreadId SteeringTaskHandle;
osThreadId CzujnikiTaskHandle;
osThreadId BTTaskHandle;
osThreadId FutabaTaskHandle;
osThreadId GovernorTaskHandle;

/* USER CODE BEGIN Variables */
osThreadId blinkTID;
osThreadId driveControlTID;
osThreadId engineTID;
osThreadId servoTID;
osThreadId SDcardTID;
/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void StartDefaultTask(void const * argument);
extern void StartLightingTask(void const * argument);
extern void StartGyroTask(void const * argument);
extern void StartBatteryManager(void const * argument);
extern void StartSteeringTask(void const * argument);
extern void StartCzujnikiTask(void const * argument);
extern void StartBTTask(void const * argument);
extern void StartFutabaTask(void const * argument);
extern void StartGovernorTask(void const * argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */
extern void blinkThread(void const * argument);
extern void driveControl(void const * argument);
extern void servoControl(void const * argument);
extern void engineControl(void const * argument);
/* USER CODE END FunctionPrototypes */

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void)
{

}

__weak unsigned long getRunTimeCounterValue(void)
{
return 0;
}
/* USER CODE END 1 */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityBelowNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of LightingTask */
  osThreadDef(LightingTask, StartLightingTask, osPriorityBelowNormal, 0, 128);
  LightingTaskHandle = osThreadCreate(osThread(LightingTask), NULL);

  /* definition and creation of GyroTask */
  osThreadDef(GyroTask, StartGyroTask, osPriorityHigh, 0, 128);
  GyroTaskHandle = osThreadCreate(osThread(GyroTask), NULL);

  /* definition and creation of BatteryManager */
  osThreadDef(BatteryManager, StartBatteryManager, osPriorityLow, 0, 128);
  BatteryManagerHandle = osThreadCreate(osThread(BatteryManager), NULL);

  /* definition and creation of SteeringTask */
  osThreadDef(SteeringTask, StartSteeringTask, osPriorityHigh, 0, 128);
  SteeringTaskHandle = osThreadCreate(osThread(SteeringTask), NULL);

  /* definition and creation of CzujnikiTask */
  osThreadDef(CzujnikiTask, StartCzujnikiTask, osPriorityNormal, 0, 128);
  CzujnikiTaskHandle = osThreadCreate(osThread(CzujnikiTask), NULL);

  /* definition and creation of BTTask */
  osThreadDef(BTTask, StartBTTask, osPriorityBelowNormal, 0, 128);
  BTTaskHandle = osThreadCreate(osThread(BTTask), NULL);

  /* definition and creation of FutabaTask */
  osThreadDef(FutabaTask, StartFutabaTask, osPriorityHigh, 0, 128);
  FutabaTaskHandle = osThreadCreate(osThread(FutabaTask), NULL);

  /* definition and creation of GovernorTask */
  osThreadDef(GovernorTask, StartGovernorTask, osPriorityHigh, 0, 256);
  GovernorTaskHandle = osThreadCreate(osThread(GovernorTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
//    osThreadDef(blink, blinkThread, osPriorityLow, 0, 128);
//    blinkTID = osThreadCreate (osThread(blink), NULL);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();

  /* USER CODE BEGIN StartDefaultTask */
	/* Infinite loop */
	for (;;) {
		if (hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED) {

			static uint16_t cnt00 = 0;
			if (cnt00++ > 9) {
				HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
				cnt00 = 0;
			}

		}
		osDelay(10);
	}
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Application */
int8_t MAIN_USB_Receive(uint8_t* Buf, uint32_t *Len) {
	if (Buf[0] == 'B')
		machine_bootloader();
	else if (Buf[0] == 'a') {
		len = sprintf((char*) usbTxBuffer,
				"YawRate\t\t= %.5f deg/s\r\nCumulativeYaw\t= %.5f deg\r\n\r\n",
				YawRate, CumulativeYaw);
		CDC_Transmit_FS(usbTxBuffer, len);
	} else if (Buf[0] == 'b') {
		len = sprintf((char*) usbTxBuffer,
				"GyroTemp\t= %.2f degC\r\n\r\n",
				temperature);
		CDC_Transmit_FS(usbTxBuffer, len);
	} else if (Buf[0] == 'c') {
		len = sprintf((char*) usbTxBuffer,
				"Voltage\t\t= %.2f Volts\r\nCurrent\t= %.2f Amps\r\nFuel\t= %.1f mAh\r\n\r\n",
				Volts_f, Amps_f, mAhs_drawn);
		CDC_Transmit_FS(usbTxBuffer, len);
	} else if (Buf[0] == 'e') {
		len =
				sprintf((char*) usbTxBuffer,
						"vleft = %d \r\nvright = %d \r\nvfwd = %d \r\ndistleft = %.1f \r\ndistright = %.1f\r\nfwdDist = %.1f \r\n\r\n",
						vleft, vright, vfwd, leftRoad, rightRoad, fwdRoad);
		CDC_Transmit_FS(usbTxBuffer, len);
	} else if (Buf[0] == 'v') {
		len = 0;
		for (int sensor = 0; sensor < VLX_SENSOR_COUNT; sensor++)
			len += sprintf((char*) usbTxBuffer + len, "vlx[%d] = %d\r\n",
					sensor+1, range[sensor]);
		len += sprintf((char*)usbTxBuffer + len, "\r\n");

		CDC_Transmit_FS(usbTxBuffer, len);
	} else if (Buf[0] == 'R')
		NVIC_SystemReset();
	return 0;
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
