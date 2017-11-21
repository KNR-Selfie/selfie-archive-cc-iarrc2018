/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2017 STMicroelectronics International N.V. 
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
#include "main.h"
#include "stm32f7xx_hal.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim10;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_uart4_rx;
DMA_HandleTypeDef hdma_uart4_tx;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;

osThreadId defaultTaskHandle;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
//ID stworzonych tematów RTOS
osThreadId blinkTID;
osThreadId driveControlTID;
osThreadId engineTID;
osThreadId servoTID;
osThreadId SDcardTID;

//deklaracja zmiennych u¿ywanych do komunikacji z Jetsonem
uint8_t j_syncByte;
uint8_t j_buffer[11];
uint16_t j_jetsonData[8];
uint8_t j_jetsonFlags[2];

//deklaracja zmiennych uzywanych do komunikacji z Aparaturka
uint8_t a_syncbyte;
uint8_t a_buffer[24];
uint16_t a_channels[16];

//inicjalizacja zmiennych globalnych - wypelnien
uint16_t duty_engine;
uint16_t duty_servo;

uint8_t prescalerOdchylka = 3;
uint8_t prescalerAngle = 7;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_UART4_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM10_Init(void);
static void MX_USART1_UART_Init(void);
void StartDefaultTask(void const * argument);
                                    
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
                                

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void blinkThread(void const * argument);
void driveControl(void const * argument);
void servoControl(void const * argument);
void engineControl(void const * argument);
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
//obsluzenie wszystkich uartow nastapi w jednej funkcji
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	if (huart->Instance == UART4) //jetson
	{
		TIM10->CNT = 0;
		if(j_syncByte==0xFF)
		{
			j_syncByte = 0xFE;
			HAL_UART_Receive_DMA(&huart4, &j_buffer, 11);
		}
		else if(j_syncByte==0xFE)
		{
			//przet³umaczenie 11x8 na u¿yteczne 8x11
			j_jetsonData[0]  = (int16_t) ((j_buffer[0] |j_buffer[1]<<8)                          	  & 0x07FF);
			j_jetsonData[1]  = (int16_t) ((j_buffer[1]>>3 |j_buffer[2]<<5)                          & 0x07FF);
			j_jetsonData[2]  = (int16_t) ((j_buffer[2]>>6 |j_buffer[3]<<2 |j_buffer[4]<<10)  		  & 0x07FF);
			j_jetsonData[3]  = (int16_t) ((j_buffer[4]>>1 |j_buffer[5]<<7)                          & 0x07FF);
			j_jetsonData[4]  = (int16_t) ((j_buffer[5]>>4 |j_buffer[6]<<4)                          & 0x07FF);
			j_jetsonData[5]  = (int16_t) ((j_buffer[6]>>7 |j_buffer[7]<<1 |j_buffer[8]<<9)  		  & 0x07FF);
			j_jetsonData[6]  = (int16_t) ((j_buffer[8]>>2 |j_buffer[9]<<6)                          & 0x07FF);
			j_jetsonData[7]  = (int16_t) ((j_buffer[9]>>5 |j_buffer[10]<<3)                         & 0x07FF);
			j_syncByte = 0xFD;
			HAL_UART_Receive_DMA(&huart4, &j_jetsonFlags, 2);
		}
		else
		{
			HAL_UART_Receive_DMA(&huart4, &j_syncByte, 1);
		}
	}

	if(huart->Instance == USART1) //aparatura
		{
			if(a_syncbyte==0x0F)
			{
				a_syncbyte=0x56;
				HAL_UART_Receive_DMA(&huart1,&a_buffer,24);
			}
			else if(a_buffer[22]==0x0)
			{
				//przet³umaczenie na u¿ytecze wartoœci 11
				a_channels[0]  = (int16_t) ((a_buffer[0]    |a_buffer[1]<<8)                          & 0x07FF);
				a_channels[1]  = (int16_t) ((a_buffer[1]>>3 |a_buffer[2]<<5)                          & 0x07FF);
				a_channels[2]  = (int16_t) ((a_buffer[2]>>6 |a_buffer[3]<<2 |a_buffer[4]<<10)  		& 0x07FF);
				a_channels[3]  = (int16_t) ((a_buffer[4]>>1 |a_buffer[5]<<7)                          & 0x07FF);
				a_channels[4]  = (int16_t) ((a_buffer[5]>>4 |a_buffer[6]<<4)                          & 0x07FF);
				a_channels[5]  = (int16_t) ((a_buffer[6]>>7 |a_buffer[7]<<1 |a_buffer[8]<<9)   		& 0x07FF);
				a_channels[6]  = (int16_t) ((a_buffer[8]>>2 |a_buffer[9]<<6)                          & 0x07FF);
				a_channels[7]  = (int16_t) ((a_buffer[9]>>5 |a_buffer[5]<<3)                         & 0x07FF);
				a_channels[8]  = (int16_t) ((a_buffer[11]   |a_buffer[12]<<8)                         & 0x07FF);
				a_channels[9]  = (int16_t) ((a_buffer[12]>>3|a_buffer[13]<<5)                         & 0x07FF);
				a_channels[10] = (int16_t) ((a_buffer[13]>>6|a_buffer[14]<<2|a_buffer[15]<<10) 		& 0x07FF);
				a_channels[11] = (int16_t) ((a_buffer[15]>>1|a_buffer[16]<<7)                         & 0x07FF);
				a_channels[12] = (int16_t) ((a_buffer[16]>>4|a_buffer[17]<<4)                         & 0x07FF);
				a_channels[13] = (int16_t) ((a_buffer[17]>>7|a_buffer[18]<<1|a_buffer[19]<<9)  		& 0x07FF);
				a_channels[14] = (int16_t) ((a_buffer[19]>>2|a_buffer[20]<<6)                         & 0x07FF);
				a_channels[15] = (int16_t) ((a_buffer[20]>>5|a_buffer[21]<<3)                         & 0x07FF);

				HAL_UART_Receive_DMA(&huart1, &a_syncbyte, 1);
			}
			else
			{
				HAL_UART_Receive_DMA(&huart1, &a_syncbyte, 1);
			}

		}

}
/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_UART4_Init();
  MX_TIM2_Init();
  MX_TIM10_Init();
  MX_USART1_UART_Init();

  /* USER CODE BEGIN 2 */
  //uruchomienie DMA na uarcie oraz PWM
  HAL_UART_Receive_DMA(&huart4, &j_syncByte, 1);
  HAL_UART_Receive_DMA(&huart1, &a_syncbyte, 1);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
  /* USER CODE END 2 */

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
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  osThreadDef(blink, blinkThread, osPriorityLow, 0, 128);
  blinkTID = osThreadCreate(osThread(blink), NULL);

  osThreadDef(drive, driveControl, osPriorityNormal, 0, 128);
  driveControlTID = osThreadCreate(osThread(drive), NULL);

  osThreadDef(servo, servoControl, osPriorityNormal, 0, 128);
  servoTID = osThreadCreate(osThread(servo), NULL);

  osThreadDef(engine, engineControl, osPriorityNormal, 0, 128);
  engineTID = osThreadCreate(osThread(engine), NULL);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
 

  /* Start scheduler */
  osKernelStart();
  
  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Activate the Over-Drive mode 
    */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_UART4;
  PeriphClkInitStruct.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInitStruct.Uart4ClockSelection = RCC_UART4CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

/* TIM2 init function */
static void MX_TIM2_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 107;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 19999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  HAL_TIM_MspPostInit(&htim2);

}

/* TIM10 init function */
static void MX_TIM10_Init(void)
{

  htim10.Instance = TIM10;
  htim10.Init.Prescaler = 21599;
  htim10.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim10.Init.Period = 49999;
  htim10.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim10.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim10) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* UART4 init function */
static void MX_UART4_Init(void)
{

  huart4.Instance = UART4;
  huart4.Init.BaudRate = 9600;
  huart4.Init.WordLength = UART_WORDLENGTH_9B;
  huart4.Init.StopBits = UART_STOPBITS_2;
  huart4.Init.Parity = UART_PARITY_EVEN;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 100000;
  huart1.Init.WordLength = UART_WORDLENGTH_9B;
  huart1.Init.StopBits = UART_STOPBITS_2;
  huart1.Init.Parity = UART_PARITY_EVEN;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_TXINVERT_INIT|UART_ADVFEATURE_RXINVERT_INIT;
  huart1.AdvancedInit.TxPinLevelInvert = UART_ADVFEATURE_TXINV_ENABLE;
  huart1.AdvancedInit.RxPinLevelInvert = UART_ADVFEATURE_RXINV_ENABLE;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);
  /* DMA1_Stream4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);
  /* DMA2_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
  /* DMA2_Stream7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB6 PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void blinkThread(void const *argument)
{
	while(1)
	{

		osDelay(500);
	}
	osThreadTerminate(NULL);
}

void driveControl(void const *argument)
{
	uint8_t transition = 0; //zbocze do hamowania
	while(1)
	{
		if(a_channels[5] == 144) //górna pozycja prze³acznika, sterowanie z aparatury
		{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET); //jeœli sterujemy z aparatury œwieci siê niebieska
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET); //czerwona siê nie œwieci
			duty_engine = (1500 + 1000 * (a_channels[1] - 1027) / (1680 - 368));
			duty_servo = (1400 + 800 * (a_channels[3] - 1000) / (1921 - 80));
			TIM2->CCR3 = duty_servo;
		}
		else if(a_channels[5] == 1024) //œrodkowa pozycja prze³¹cznika, jazda autonomiczna
		{
			HAL_TIM_Base_Start_IT(&htim10); // timer od sprawdzania komunikacji
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET); //jeœli jeŸdzimy autonomicznie œwieci siê czerwona
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET); //a niebieska nie
			//jezeli jest komunikacja na linii Jetson <-> STM
			if(j_syncByte == 254 || j_syncByte == 253 || j_syncByte == 255)
			{
				//gdy brak wykrytej linii stop lub zbocze opadajace -> nadaj kierunek jazdy -> jedz
				if (!(j_jetsonFlags[0] & 0x80) || !(transition))
				{
					if (j_jetsonData[0] == 0)
					{
						duty_servo = 1400 + j_jetsonData[1] * prescalerOdchylka + (j_jetsonData[2] - 90) * prescalerAngle;
						if (duty_servo > 1700) duty_servo = 1700;
					}
					else if (j_jetsonData[0] == 1)
					{
						duty_servo = 1400 - j_jetsonData[1] * prescalerOdchylka - (90 - j_jetsonData[2]) * prescalerAngle;
						if (duty_servo < 1100) duty_servo = 1100;
					}
					duty_engine = (uint16_t)(1610);
					transition = 1;
				}
				//gdy jest wykryta linia stop, gdy odleglosc od linii jest niemala i gdy zbocze jest narastajace -> hamuj
				else if((j_jetsonData[3] > 50) && (j_jetsonFlags[0] & 0x80) && (transition))
				{
					duty_engine = (1610 + (j_jetsonData[1] >> 5) + ((abs(j_jetsonData[2] - 90) * 10) >> 4)) - (480 -j_jetsonData[3])/5;
				}
				//gdy jest wykryta linia stop, gdy odleglosc od linii jest mala i gdy zbocze jest narastajace -> zatrzymaj sie
				else if((j_jetsonData[3] > 0) && (j_jetsonData[3] <= 50) && (j_jetsonFlags[0] & 0x80) && (transition))
				{
					duty_engine = 1450;
					osDelay(2000);
					transition = 0;
				}
			}
			else if(j_syncByte == 200) //je¿eli nie istnieje Jetson <-> STM, wylacz naped (wartosc j_syncByte = 200 jest ustawiana przez TIMER10)
			{TIM2->CCR4 = 1460;}

			TIM2->CCR3 = duty_servo;
		}
		else if(a_channels[5] == 1904) //dolna pozycja prze³acznika, tryb pó³autonomiczny
		{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET); //jeœli jeŸdzimy pó³autonomicznie œwieci siê czerwona
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET); //i niebieska te¿
			duty_engine = (1500 + 1000 * (a_channels[1] - 1027) / (1680 - 368));
			if (j_jetsonData[0] == 0)
			{
				duty_servo = 1400 + j_jetsonData[1] * prescalerOdchylka + (j_jetsonData[2] - 90) * prescalerAngle;
				if (duty_servo > 1700) duty_servo = 1700;
			}
			else if (j_jetsonData[0] == 1)
			{
				duty_servo = 1400 - j_jetsonData[1] * prescalerOdchylka - (90 - j_jetsonData[2]) * prescalerAngle;
				if (duty_servo < 1100) duty_servo = 1100;
			}
			TIM2->CCR3 = duty_servo;
		}

	}
	osThreadTerminate(NULL);
}

void servoControl(void const *argument)
{
	duty_servo = 1400;
	while(1)
	{
		//TIM2->CCR3 = duty_servo;
	}
	osThreadTerminate(NULL);
}

void engineControl(void const *argument)
{
	duty_engine = 1500;

	//uruchomienie ESC
	TIM2->CCR4 = 2000;
	osDelay(1000);
	TIM2->CCR4 = 1500;
	osDelay(2000);

	//zabezpieczenie przed jazd¹ do ty³u
	TIM2->CCR4 = 1520;
	osDelay(50);
	while(1)
	{
		TIM2->CCR4 = duty_engine;
	}
	osThreadTerminate(NULL);
}


/* USER CODE END 4 */

/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{

  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */ 
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
/* USER CODE BEGIN Callback 0 */

/* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
/* USER CODE BEGIN Callback 1 */
  if(htim->Instance == TIM10){ //timer 10 sprawdzajacy czy jest komunikacja Jetson<->STM. CNT zerowany w obsludze uarta
  	j_syncByte = 200;
  }
/* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
