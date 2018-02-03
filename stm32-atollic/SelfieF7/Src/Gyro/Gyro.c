/*
 * Gyro.cpp
 *
 *  Created on: 21.11.2017
 *      Author: mice
 */


#include "cmsis_os.h"
#include "spi.h"
#include "gpio.h"
#include "Gyro.h"
#include "mpu6000.h"
#include "main.h"
#include "Filtering.h"
#include "Lighting.h"

#include "stm32f7xx_hal.h"



//osThreadId GyroTaskHandle;
xSemaphoreHandle SemaphoreDRDY = NULL;
extern osThreadId GyroTaskHandle;

#define _SPI_CLOCK_FAST 			SPI_BAUDRATEPRESCALER_4
#define _SPI_CLOCK_INITIALIZATON 	SPI_BAUDRATEPRESCALER_256

#define CAL_CYCLES 16000

#define GYRO_SCALING 65.5f
#define ACC_SCALING 4096

uint8_t GyroTxBuffer[32];
uint8_t GyroRxBuffer[32];

uint8_t gyro_on=0;
uint8_t calibration_mode=0;
int32_t cal_data[3] = { 0, 0, 0 };
float GYRO_ZRO[3]={0, 0, 0};

int16_t gyro_raw[3];
int16_t acc_raw[3];
int16_t temp_raw;
float g_rates[3];
float temperature;
float rates[3];
float rates_raw[3];

float CumulativeYaw = 0.f;
float YawRate = 0.f;

uint32_t gyroLooptime = 8000; // Gyro Loop Rate in Hz.

filter_t gyroLPF[3];
static uint8_t gyroLPF_Hz[3] = {60, 60, 60};

extern void StartGyroTask(void const * argument);

HAL_StatusTypeDef gyro_write(uint8_t gyro_reg, uint8_t data);
HAL_StatusTypeDef gyro_data_read(uint8_t reg_addr, uint8_t *data, uint32_t size);
void gyro_data_conv(uint8_t *data);
void gyro_calfunction(void);
void gyro_zeroing(void);
void gyro_filtering(void);
void mpuInit(void);


void StartGyroTask(void const * argument){
	vSemaphoreCreateBinary(SemaphoreDRDY);
	osSemaphoreWait(SemaphoreDRDY, osWaitForever);
	for (int axis = 0; axis < 3; axis++)
		lpf_filter_init(&gyroLPF[axis], gyroLPF_Hz[axis], gyroLooptime);

	mpuInit();
	calibration_mode = 1;
	while (1) {
            osSemaphoreWait (SemaphoreDRDY, osWaitForever);
		if (gyro_on) {
                    gyro_data_read (MPU_RA_ACCEL_XOUT_H | 0x80, GyroRxBuffer, 14);
                    gyro_data_conv (GyroRxBuffer);
                    gyro_calfunction ();
                    gyro_zeroing ();
                    gyro_filtering ();

                    YawRate = rates[2];
                    CumulativeYaw+=YawRate/(float)gyroLooptime;
			static uint16_t cnt00 = 0;
			if (cnt00++ > 3999) {
				HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
				cnt00 = 0;
			}
		}

	}
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == MPU_DRDY_Pin && gyro_on) // Gyro Data READY
			{
		osSemaphoreRelease(SemaphoreDRDY);
//		static int exti_licznik = 0;
//		if (++exti_licznik > 4000) {
//			HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
//			exti_licznik = 0;
//		}
	}
}
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
	if (hspi->Instance == hspi1.Instance) {
		osSignalSet(GyroTaskHandle, 0x01);
	}
}
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) {
	if (hspi->Instance == hspi1.Instance) {
		osSignalSet(GyroTaskHandle, 0x02);
	}
}
void mpuInit(){
	gyro_on = 0;
	osDelay(1);
	HAL_SPI_DeInit(&hspi1);
	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET);
	Selfie_SPI1_Init(_SPI_CLOCK_INITIALIZATON);
	osDelay(1);
	// Device Reset
	gyro_write(MPU_RA_PWR_MGMT_1, BIT_H_RESET);
	osDelay(150);
	gyro_write(MPU_RA_SIGNAL_PATH_RESET, BIT_GYRO | BIT_ACC | BIT_TEMP);
	osDelay(150);

    gyro_write(MPU_RA_PWR_MGMT_1, MPU_CLK_SEL_PLLGYROZ);
	osDelay(1);
	// Disable Primary I2C Interface
	gyro_write(MPU_RA_USER_CTRL, BIT_I2C_IF_DIS);
	osDelay(1);
	gyro_write(MPU_RA_PWR_MGMT_2, 0x00);
	osDelay(1);
	// Accel Sample Rate 1kHz
	// Gyroscope Output Rate =  1kHz when the DLPF is enabled
	gyro_write(0x1a, BITS_DLPF_CFG_2100HZ_NOLPF);
	osDelay(1);
	gyro_write(MPU_RA_SMPLRT_DIV, 0);
	osDelay(1);
	// Gyro +/- 1000 DPS Full Scale
	gyro_write(MPU_RA_GYRO_CONFIG, INV_FSR_500DPS << 3);
	osDelay(1);
	// Accel +/- 8 G Full Scale
	gyro_write(MPU_RA_ACCEL_CONFIG, INV_FSR_8G << 3);
	osDelay(1);

	gyro_write(MPU_RA_INT_PIN_CFG,
			0 << 7 | 0 << 6 | 0 << 5 | 1 << 4 | 0 << 3 | 0 << 2 | 0 << 1
					| 0 << 0);  // INT_ANYRD_2CLEAR
	osDelay(1);

	gyro_write(MPU_RA_INT_ENABLE, (1 << 0));
	osDelay(1);

	HAL_SPI_DeInit(&hspi1);
	osDelay(1);
	Selfie_SPI1_Init(_SPI_CLOCK_FAST);
	gyro_on = 1;
	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);
    }
void mpuInit2(){
	gyro_on = 0;
	HAL_Delay(1);
	HAL_SPI_DeInit(&hspi1);
	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET);
	Selfie_SPI1_Init(_SPI_CLOCK_INITIALIZATON);
	HAL_Delay(1);
	// Device Reset
	gyro_write(MPU_RA_PWR_MGMT_1, BIT_H_RESET);
	HAL_Delay(150);
	gyro_write(MPU_RA_SIGNAL_PATH_RESET, BIT_GYRO | BIT_ACC | BIT_TEMP);
	HAL_Delay(150);

    gyro_write(MPU_RA_PWR_MGMT_1, MPU_CLK_SEL_PLLGYROZ);
    HAL_Delay(1);
	// Disable Primary I2C Interface
	gyro_write(MPU_RA_USER_CTRL, BIT_I2C_IF_DIS);
	HAL_Delay(1);
	gyro_write(MPU_RA_PWR_MGMT_2, 0x00);
	HAL_Delay(1);
	// Accel Sample Rate 1kHz
	// Gyroscope Output Rate =  1kHz when the DLPF is enabled
	gyro_write(0x1a, BITS_DLPF_CFG_256HZ);
	HAL_Delay(1);
	gyro_write(MPU_RA_SMPLRT_DIV, 0);
	HAL_Delay(1);
	// Gyro +/- 1000 DPS Full Scale
	gyro_write(MPU_RA_GYRO_CONFIG, INV_FSR_2000DPS << 3);
	HAL_Delay(1);
	// Accel +/- 8 G Full Scale
	gyro_write(MPU_RA_ACCEL_CONFIG, INV_FSR_8G << 3);
	HAL_Delay(1);

	gyro_write(MPU_RA_INT_PIN_CFG,
			0 << 7 | 0 << 6 | 0 << 5 | 1 << 4 | 0 << 3 | 0 << 2 | 0 << 1
					| 0 << 0);  // INT_ANYRD_2CLEAR
	HAL_Delay(1);

	gyro_write(MPU_RA_INT_ENABLE, (1 << 0));
	HAL_Delay(1);

	HAL_SPI_DeInit(&hspi1);
	HAL_Delay(1);
	Selfie_SPI1_Init(_SPI_CLOCK_FAST);
	gyro_on = 1;
	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);
    }
HAL_StatusTypeDef gyro_write(uint8_t gyro_reg, uint8_t data) {
	HAL_StatusTypeDef status = HAL_OK;
    osSignalSet(GyroTaskHandle, 0x00);
    HAL_GPIO_WritePin(MPU_CS_GPIO_Port, MPU_CS_Pin, GPIO_PIN_RESET);
    status |= HAL_SPI_Transmit_IT(&hspi1, (uint8_t*)&gyro_reg, 1);
	osSignalWait(0x01, 1);
	osSignalSet(GyroTaskHandle, 0x00);
	status |= HAL_SPI_Transmit_IT(&hspi1, (uint8_t*)&data, 1);
	osSignalWait(0x01, 1);
    HAL_GPIO_WritePin(MPU_CS_GPIO_Port, MPU_CS_Pin, GPIO_PIN_SET);
    osSignalSet(GyroTaskHandle, 0x00);

    return status;
}
HAL_StatusTypeDef gyro_data_read(uint8_t reg_addr, uint8_t *data, uint32_t size) {
	HAL_StatusTypeDef status = HAL_OK;
	osSignalSet(GyroTaskHandle, 0x00);
	HAL_GPIO_WritePin(MPU_CS_GPIO_Port, MPU_CS_Pin, GPIO_PIN_RESET);
	status |= HAL_SPI_Transmit_IT(&hspi1, &reg_addr, 1);
	osSignalWait(0x01, 1);
	status |= HAL_SPI_Receive_IT(&hspi1, data, size);
	osSignalWait(0x02, 1);
	HAL_GPIO_WritePin(MPU_CS_GPIO_Port, MPU_CS_Pin, GPIO_PIN_SET);
	osSignalSet(GyroTaskHandle, 0x00);

	return status;
}
void gyro_data_conv(uint8_t * data)  {
	acc_raw[0] = data[0] << 8 | data[1];
	acc_raw[1] = data[2] << 8 | data[3];
	acc_raw[2] = data[4] << 8 | data[5];

    gyro_raw[0] = data[8] << 8 | data[9];
    gyro_raw[1] = data[10] << 8 | data[11];
    gyro_raw[2] = -(data[12] << 8 | data[13]);

	temp_raw = data[6] << 8 | data[7];

	temperature = (float)temp_raw/340.f + 36.53f;
    for (int axis = 0; axis < 3; axis++) {
        rates_raw[axis] = gyro_raw[axis] / GYRO_SCALING;
        g_rates[axis] = acc_raw[axis] / ACC_SCALING;
    }
}
void gyro_calfunction(void) {
    if (calibration_mode) {
        static uint32_t cal_inc = 1;
        for (int axis = 0; axis < 3; axis++)
            cal_data[axis] += gyro_raw[axis];
        if (cal_inc++ >= CAL_CYCLES) {
            cal_inc = 0;
            for (int axis = 0; axis < 3; axis++) {
                GYRO_ZRO[axis] = (float)(cal_data[axis] / CAL_CYCLES)/GYRO_SCALING;
                cal_data[axis] = 0;
            }
            calibration_mode = 0;
        }
    }
}
void gyro_zeroing(void) {
    for (int axis = 0; axis < 3; axis++)
        rates_raw[axis] -= GYRO_ZRO[axis];
}
void gyro_filtering(void) {
    for (int axis = 0; axis < 3; axis++) {
        rates[axis] = filter_apply(&gyroLPF[axis], rates_raw[axis]);
    }
}
