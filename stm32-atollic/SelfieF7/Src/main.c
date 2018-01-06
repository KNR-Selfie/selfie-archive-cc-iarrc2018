/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************

  * Dziala mi (KOT to chuj)
  * Mi też działa (Perciak to chuj)
  * Mi też działa (każdy Mateusz chuj)
  * Z Attolica(Janusz chuj)
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
#include "Gyro.h"
#include "Enc.h"
#include "BT.h"
#include "Czujniki.h"
#include "MotorControl.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
////////testy BT
uint8_t choice = 0;
int tmp = 0;

uint16_t dane[4];
uint16_t data[20]; // Tablica przechowujaca wysylana wiadomosc.
int size;
uint8_t tekst[20];

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

///////////semaphore potrzebny aby go zwolnic w UARCie
extern osSemaphoreId DriveControlSemaphoreHandle;


//deklaracja zmiennych u¿ywanych do komunikacji z Odroidem
uint8_t j_syncByte;
uint8_t j_buffer[11];
uint16_t j_jetsonData[8];
uint8_t j_jetsonFlags[2];
uint8_t synchroniseUARTOdroid = 0;

//deklaracja zmiennych uzywanych do komunikacji z Aparaturka
uint8_t a_syncbyte;
uint8_t a_buffer[24];
uint16_t a_channels[16];
uint8_t synchroniseUARTAparatura = 0;

////usb/gyro?
uint8_t sem_init = 0;
xSemaphoreHandle Tim7Semaphore = NULL;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void blinkThread(void const * argument);
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
  MX_TIM10_Init();
  MX_TIM5_Init();
  MX_TIM3_Init();

  /* USER CODE BEGIN 2 */
	HAL_UART_Receive_DMA(&huart4, &j_syncByte, 1);
	HAL_UART_Receive_DMA(&huart1, &a_syncbyte, 1);
	HAL_UART_Receive_DMA(&huart6, &choice, 1);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start(&htim5, TIM_CHANNEL_ALL);

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
	while (1) {
		osDelay(1000);
	}
	osThreadTerminate(NULL);
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

    if (huart->Instance == UART4) //odroid
    {

        TIM10->CNT = 0;
        if(j_syncByte==0xFF && synchroniseUARTOdroid == 0)
        {
            synchroniseUARTOdroid = 1;
            HAL_UART_Receive_DMA(&huart4, j_buffer, 11);
        }
        else if(j_syncByte != 0xFF && synchroniseUARTOdroid == 0)
            HAL_UART_Receive_DMA(&huart4, &j_syncByte, 1);

        else if(synchroniseUARTOdroid == 1)
        {

            synchroniseUARTOdroid =2;
            HAL_UART_Receive_DMA(&huart4, j_jetsonFlags, 2);
        }
        else if(synchroniseUARTOdroid == 2)
        {
        	if(j_jetsonFlags[1] == 0xFE && ((j_jetsonFlags[0] & 0x0003) == 0)){
                //przetlumaczenie 11x8 na uzyteczne 8x11
                j_jetsonData[0]  = (int16_t) ((j_buffer[0] |j_buffer[1]<<8)                               & 0x07FF);
                j_jetsonData[1]  = (int16_t) ((j_buffer[1]>>3 |j_buffer[2]<<5)                          & 0x07FF);
                j_jetsonData[2]  = (int16_t) ((j_buffer[2]>>6 |j_buffer[3]<<2 |j_buffer[4]<<10)           & 0x07FF);
                j_jetsonData[3]  = (int16_t) ((j_buffer[4]>>1 |j_buffer[5]<<7)                          & 0x07FF);
                j_jetsonData[4]  = (int16_t) ((j_buffer[5]>>4 |j_buffer[6]<<4)                          & 0x07FF);
                j_jetsonData[5]  = (int16_t) ((j_buffer[6]>>7 |j_buffer[7]<<1 |j_buffer[8]<<9)            & 0x07FF);
                j_jetsonData[6]  = (int16_t) ((j_buffer[8]>>2 |j_buffer[9]<<6)                          & 0x07FF);
                j_jetsonData[7]  = (int16_t) ((j_buffer[9]>>5 |j_buffer[10]<<3)                         & 0x07FF);
        	}
        	synchroniseUARTOdroid = 0;
            HAL_UART_Receive_DMA(&huart4, &j_syncByte, 1);
            osSemaphoreRelease(DriveControlSemaphoreHandle);
        }
    }

    if(huart->Instance == USART1) //aparatura
        {

            if(a_syncbyte==0x0F && synchroniseUARTAparatura == 0)
            {
                synchroniseUARTAparatura = 1;
                HAL_UART_Receive_DMA(&huart1,a_buffer,24);
            }
            else if(a_syncbyte != 0x0F && synchroniseUARTAparatura == 0)
                HAL_UART_Receive_DMA(&huart4, &j_syncByte, 1);
            else if(a_buffer[22]==0x00 && synchroniseUARTAparatura == 1)
            {
                //przetlumaczenie na uzytecze wartosci 11
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

                synchroniseUARTAparatura = 0;
                HAL_UART_Receive_DMA(&huart1, &a_syncbyte, 1);
                osSemaphoreRelease(DriveControlSemaphoreHandle);
            }
            else
            {
                synchroniseUARTAparatura = 0;
                HAL_UART_Receive_DMA(&huart1, &a_syncbyte, 1);
            }

        }

    if(huart->Instance == USART6){ ////////////////Bluetooth HC-05
        size = sprintf(tekst, "Wybierz co chcesz zrobic: a - nast b - dane \n\r");
        HAL_UART_Transmit_DMA(&huart6, tekst, size);


//    	if (choice == 122) {
//    	        size = sprintf(tekst,
//    	                "Wybierz co chcesz zrobic: a - nast b - dane \n\r");
//    	        HAL_UART_Transmit_IT(&huart6, tekst, size);
//    	        choice = 0;
//    	        sendflag = 0;
//    	        HAL_UART_Receive_IT(&huart6, &tmp, 1);
//
//    	    } else if (tmp == 97) {
//    	        tmp = 0;
//    	        size = sprintf(tekst,
//    	                "Ustaw  a-r_kpPredkosc, b-ki , c-r_kpPredkosc2, d-ki2, e - speed\n\r");
//    	        flag = 1;
//    	        HAL_UART_Transmit_IT(&huart6, tekst, size);
//    	        HAL_UART_Receive_IT(&huart6, &nr, 1);
//    	    } else if (tmp == 98) { //podmenu wysylania
//    	        sendflag = 1; // flaga pomocnicza - uruchamia wysy�anie danych z STM
//    	        flag = 0;
//    	        tmp = 0;
//    	        HAL_UART_Receive_IT(&huart6, &choice, 1);
//    	    }
//
//    	    else if (nr > 10 && flag == 1) {
//    	        flag = 0;
//    	        size = sprintf(tekst, "wartosc:\n\r ");
//    	        HAL_UART_Transmit_IT(&huart6, tekst, size);
//    	        HAL_UART_Receive_IT(&huart6, &value, 6);
//    	    }
//
//    	    else if (value[0] != 0) {
//    	        switch (nr) {
//    	        case 97:
//    	            for (int i = 0; i < 6; i++) {
//    	                tab[i] = (char) value[i];
//    	            }
//    	            r_kpPredkosc = atof(tab);
//    	            value[0] = 0;
//    	            break;
//
//    	        case 98:
//    	            for (int i = 0; i < 6; i++) {
//    	                tab[i] = (char) value[i];
//    	            }
//    	            angle = atof(tab);
//    	            value[0] = 0;
//    	            //przypisz wartosc nastawy
//    	            break;
//    	        case 99:
//    	            for (int i = 0; i < 6; i++) {
//    	                tab[i] = (char) value[i];
//    	            }
//    	            angle = atof(tab);
//    	            value[0] = 0;
//    	            //przypisz wartosc nastawy
//    	            break;
//    	        case 100:
//    	            for (int i = 0; i < 6; i++) {
//    	                tab[i] = (char) value[i];
//    	            }
//    	            angle = atof(tab);
//    	            value[0] = 0;
//    	            //przypisz wartosc nastawy
//    	            break;
//    	        case 101:
//    	            for (int i = 0; i < 6; i++) {
//    	                tab[i] = (char) value[i];
//    	            }
//
//    	            angle = atof(tab);
//    	            speed = (int) angle;
//    	            value[0] = 0;
//    	            //przypisz wartosc nastawy
//    	            break;
//
//    	        default:
//    	            break;
//
//    	        }
//    	        HAL_UART_Receive_IT(&huart6, &choice, 1);
//    	    }

    	    HAL_UART_Receive_DMA(&huart6, &choice, 1);
    }


}
void machine_bootloader(void) {
	HAL_NVIC_DisableIRQ(TIM1_CC_IRQn);
	vTaskSuspendAll();
	vTaskEndScheduler();
	__disable_irq();
	HAL_RCC_DeInit();

	HAL_DeInit();

	HAL_DeInit();

	__set_MSP(*((uint32_t*) BOOTLOADER_STACK_POINTER));

	((void (*)(void)) *((uint32_t*) (BOOTLOADER_STACK_POINTER + 4ul)))();

	while (1);
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
  if (htim->Instance == TIM7) {
      static int licznik = 0;
      if(++licznik>50 && sem_init)
      {
    	  licznik=0;
//    	  osSemaphoreRelease(Tim7Semaphore);
      }
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
