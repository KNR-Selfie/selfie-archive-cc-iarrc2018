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
#include "adc.h"
#include "can.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "Lighting.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

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

int rgb[22][3];

int is1 = 1;
int16_t scounter1 = 1;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void blinkThread(void const * argument);
void driveControl(void const * argument);
void servoControl(void const * argument);
void engineControl(void const * argument);
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

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
  MX_USART1_UART_Init();
  MX_TIM4_Init();
  MX_TIM10_Init();
  MX_TIM5_Init();
  MX_TIM3_Init();

  /* USER CODE BEGIN 2 */
  //uruchomienie DMA na uarcie oraz PWM

    ws2812_init();
    HAL_UART_Receive_DMA(&huart4, &j_syncByte, 1);
    HAL_UART_Receive_DMA(&huart1, &a_syncbyte, 1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
  /* USER CODE END 2 */

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();

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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
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

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2
                              |RCC_PERIPHCLK_USART3|RCC_PERIPHCLK_USART6
                              |RCC_PERIPHCLK_UART4|RCC_PERIPHCLK_UART5
                              |RCC_PERIPHCLK_UART7|RCC_PERIPHCLK_UART8
                              |RCC_PERIPHCLK_I2C2|RCC_PERIPHCLK_I2C4
                              |RCC_PERIPHCLK_CLK48;
  PeriphClkInitStruct.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInitStruct.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInitStruct.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  PeriphClkInitStruct.Uart4ClockSelection = RCC_UART4CLKSOURCE_PCLK1;
  PeriphClkInitStruct.Uart5ClockSelection = RCC_UART5CLKSOURCE_PCLK1;
  PeriphClkInitStruct.Usart6ClockSelection = RCC_USART6CLKSOURCE_PCLK2;
  PeriphClkInitStruct.Uart7ClockSelection = RCC_UART7CLKSOURCE_PCLK1;
  PeriphClkInitStruct.Uart8ClockSelection = RCC_UART8CLKSOURCE_PCLK1;
  PeriphClkInitStruct.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
  PeriphClkInitStruct.I2c4ClockSelection = RCC_I2C4CLKSOURCE_PCLK1;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48SOURCE_PLL;
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

/* USER CODE BEGIN 4 */
void blinkThread(void const *argument)
{
    while(1)
    {
        osDelay(1);
    }
    osThreadTerminate(NULL);
}

void driveControl(void const *argument)
{
    uint8_t transition = 0; //zbocze do hamowania
    while (1)
        {
            if (a_channels[5] == 144) //górna pozycja prze³acznika, sterowanie z aparatury
                {
                    HAL_GPIO_WritePin (GPIOB, GPIO_PIN_7, GPIO_PIN_SET); //jeœli sterujemy z aparatury œwieci siê niebieska
                    HAL_GPIO_WritePin (GPIOB, GPIO_PIN_6, GPIO_PIN_RESET); //czerwona siê nie œwieci
                    duty_engine = (1500
                            + 1000 * (a_channels[1] - 1027) / (1680 - 368));
                    duty_servo = (1400
                            + 800 * (a_channels[3] - 1000) / (1921 - 80));
                    TIM2->CCR3 = duty_servo;
                }
            else if (a_channels[5] == 1024) //œrodkowa pozycja prze³¹cznika, jazda autonomiczna
                {
                    HAL_TIM_Base_Start_IT (&htim10); // timer od sprawdzania komunikacji
                    HAL_GPIO_WritePin (GPIOB, GPIO_PIN_6, GPIO_PIN_SET); //jeœli jeŸdzimy autonomicznie œwieci siê czerwona
                    HAL_GPIO_WritePin (GPIOB, GPIO_PIN_7, GPIO_PIN_RESET); //a niebieska nie
                    //jezeli jest komunikacja na linii Jetson <-> STM
                    if (j_syncByte == 254 || j_syncByte == 253
                            || j_syncByte == 255)
                        {
                            //gdy brak wykrytej linii stop lub zbocze opadajace -> nadaj kierunek jazdy -> jedz
                            if (!(j_jetsonFlags[0] & 0x80) || !(transition))
                                {
                                    if (j_jetsonData[0] == 0)
                                        {
                                            duty_servo = 1400
                                                    + j_jetsonData[1]
                                                            * prescalerOdchylka
                                                    + (j_jetsonData[2] - 90)
                                                            * prescalerAngle;
                                            if (duty_servo > 1700)
                                                duty_servo = 1700;
                                        }
                                    else if (j_jetsonData[0] == 1)
                                        {
                                            duty_servo = 1400
                                                    - j_jetsonData[1]
                                                            * prescalerOdchylka
                                                    - (90 - j_jetsonData[2])
                                                            * prescalerAngle;
                                            if (duty_servo < 1100)
                                                duty_servo = 1100;
                                        }
                                    duty_engine = (uint16_t) (1610);
                                    transition = 1;
                                }
                            //gdy jest wykryta linia stop, gdy odleglosc od linii jest niemala i gdy zbocze jest narastajace -> hamuj
                            else if ((j_jetsonData[3] > 50)
                                    && (j_jetsonFlags[0] & 0x80)
                                    && (transition))
                                {
                                    duty_engine = (1610 + (j_jetsonData[1] >> 5)
                                            + ((abs (j_jetsonData[2] - 90) * 10)
                                                    >> 4))
                                            - (480 - j_jetsonData[3]) / 5;
                                }
                            //gdy jest wykryta linia stop, gdy odleglosc od linii jest mala i gdy zbocze jest narastajace -> zatrzymaj sie
                            else if ((j_jetsonData[3] > 0)
                                    && (j_jetsonData[3] <= 50)
                                    && (j_jetsonFlags[0] & 0x80)
                                    && (transition))
                                {
                                    duty_engine = 1450;
                                    osDelay (2000);
                                    transition = 0;
                                }
                        }
                    else if (j_syncByte == 200) //je¿eli nie istnieje Jetson <-> STM, wylacz naped (wartosc j_syncByte = 200 jest ustawiana przez TIMER10)
                        {
                            TIM2->CCR4 = 1460;
                        }

                    TIM2->CCR3 = duty_servo;
                }
            else if (a_channels[5] == 1904) //dolna pozycja prze³acznika, tryb pó³autonomiczny
                {
                    HAL_GPIO_WritePin (GPIOB, GPIO_PIN_6, GPIO_PIN_SET); //jeœli jeŸdzimy pó³autonomicznie œwieci siê czerwona
                    HAL_GPIO_WritePin (GPIOB, GPIO_PIN_7, GPIO_PIN_SET); //i niebieska te¿
                    duty_engine = (1500
                            + 1000 * (a_channels[1] - 1027) / (1680 - 368));
                    if (j_jetsonData[0] == 0)
                        {
                            duty_servo = 1400
                                    + j_jetsonData[1] * prescalerOdchylka
                                    + (j_jetsonData[2] - 90) * prescalerAngle;
                            if (duty_servo > 1700)
                                duty_servo = 1700;
                        }
                    else if (j_jetsonData[0] == 1)
                        {
                            duty_servo = 1400
                                    - j_jetsonData[1] * prescalerOdchylka
                                    - (90 - j_jetsonData[2]) * prescalerAngle;
                            if (duty_servo < 1100)
                                duty_servo = 1100;
                        }
                    TIM2->CCR3 = duty_servo;
                }
            if (a_channels[4] > 1027)
                {
                    scounter1 += 1 * is1;

                    if (scounter1 >= 999 || scounter1 <= 0)
                        is1 *= -1;

                    for (int indeks = 0; indeks < 22; indeks++)
                        {
                            hsi2rgb (
                                    fabs (360 * scounter1 / 999
                                            - 5 * (indeks - 2)),
                                    1, 1, &rgb[indeks][0]);
                            ws2812_set_color (indeks, rgb[indeks][0],
                                              rgb[indeks][1], rgb[indeks][2]);
                        }
                }
            else
                {



                    ws2812_set_color (1, 255, 255, 255);
                    ws2812_set_color (2, 255, 255, 255);
                    ws2812_set_color (3, 255, 255, 255);
                    ws2812_set_color (4, 0, 0, 0);
                    ws2812_set_color (5, 0, 0, 0);
                    ws2812_set_color (6, 0, 0, 0);
                    ws2812_set_color (7, 255, 255, 255);
                    ws2812_set_color (8, 255, 255, 255);
                    ws2812_set_color (9, 255, 255, 255);

                    if (a_channels[1] < 1027)
                        {
                            ws2812_set_color (12, 255, 0, 0);
                            ws2812_set_color (13, 255, 0, 0);
                            ws2812_set_color (14, 255, 0, 0);

                            ws2812_set_color (20, 255, 0, 0);
                            ws2812_set_color (19, 255, 0, 0);
                            ws2812_set_color (18, 255, 0, 0);

                        }
                    else
                        {
                            ws2812_set_color (12, 50, 0, 0);
                            ws2812_set_color (13, 50, 0, 0);
                            ws2812_set_color (14, 50, 0, 0);

                            ws2812_set_color (20, 50, 0, 0);
                            ws2812_set_color (19, 50, 0, 0);
                            ws2812_set_color (18, 50, 0, 0);
                        }
                    ws2812_set_color (15, 0, 0, 0);
                    ws2812_set_color (16, 0, 0, 0);
                    ws2812_set_color (17, 0, 0, 0);
                    if (a_channels[3] > 1250)
                        {
                            ws2812_set_color (11, 255, 120, 0);
                            ws2812_set_color (10, 255, 120, 0);
                            ws2812_set_color (0, 0, 0, 0);
                            ws2812_set_color (21, 0, 0, 0);
                        }
                    else if (a_channels[3] < 800)
                        {
                            ws2812_set_color (0, 255, 120, 0);
                            ws2812_set_color (21, 255, 120, 0);
                            ws2812_set_color (11, 0, 0, 0);
                            ws2812_set_color (10, 0, 0, 0);
                        }
                    else
                        {
                            ws2812_set_color (0, 0, 0, 0);
                            ws2812_set_color (21, 0, 0, 0);
                            ws2812_set_color (11, 0, 0, 0);
                            ws2812_set_color (10, 0, 0, 0);
                    }

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
            j_jetsonData[0]  = (int16_t) ((j_buffer[0] |j_buffer[1]<<8)                               & 0x07FF);
            j_jetsonData[1]  = (int16_t) ((j_buffer[1]>>3 |j_buffer[2]<<5)                          & 0x07FF);
            j_jetsonData[2]  = (int16_t) ((j_buffer[2]>>6 |j_buffer[3]<<2 |j_buffer[4]<<10)           & 0x07FF);
            j_jetsonData[3]  = (int16_t) ((j_buffer[4]>>1 |j_buffer[5]<<7)                          & 0x07FF);
            j_jetsonData[4]  = (int16_t) ((j_buffer[5]>>4 |j_buffer[6]<<4)                          & 0x07FF);
            j_jetsonData[5]  = (int16_t) ((j_buffer[6]>>7 |j_buffer[7]<<1 |j_buffer[8]<<9)            & 0x07FF);
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
                a_channels[2]  = (int16_t) ((a_buffer[2]>>6 |a_buffer[3]<<2 |a_buffer[4]<<10)       & 0x07FF);
                a_channels[3]  = (int16_t) ((a_buffer[4]>>1 |a_buffer[5]<<7)                          & 0x07FF);
                a_channels[4]  = (int16_t) ((a_buffer[5]>>4 |a_buffer[6]<<4)                          & 0x07FF);
                a_channels[5]  = (int16_t) ((a_buffer[6]>>7 |a_buffer[7]<<1 |a_buffer[8]<<9)        & 0x07FF);
                a_channels[6]  = (int16_t) ((a_buffer[8]>>2 |a_buffer[9]<<6)                          & 0x07FF);
                a_channels[7]  = (int16_t) ((a_buffer[9]>>5 |a_buffer[5]<<3)                         & 0x07FF);
                a_channels[8]  = (int16_t) ((a_buffer[11]   |a_buffer[12]<<8)                         & 0x07FF);
                a_channels[9]  = (int16_t) ((a_buffer[12]>>3|a_buffer[13]<<5)                         & 0x07FF);
                a_channels[10] = (int16_t) ((a_buffer[13]>>6|a_buffer[14]<<2|a_buffer[15]<<10)      & 0x07FF);
                a_channels[11] = (int16_t) ((a_buffer[15]>>1|a_buffer[16]<<7)                         & 0x07FF);
                a_channels[12] = (int16_t) ((a_buffer[16]>>4|a_buffer[17]<<4)                         & 0x07FF);
                a_channels[13] = (int16_t) ((a_buffer[17]>>7|a_buffer[18]<<1|a_buffer[19]<<9)       & 0x07FF);
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
/* USER CODE END 4 */

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
