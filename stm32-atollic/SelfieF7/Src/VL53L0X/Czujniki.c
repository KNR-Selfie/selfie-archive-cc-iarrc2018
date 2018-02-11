/*
 * Czujniki.c
 *

 */

#include "Czujniki.h"
#include "VL53L0X.h"
#include "cmsis_os.h"
#include "tim.h"
#include "i2c.h"
#include "gpio.h"
#include "Governor.h"
#include "Battery.h"
#include "Lighting.h"

//xSemaphoreHandle i2c2_semaphore = NULL;
//#define EXPANDER_ADRESS 0x40
uint16_t thresholdPrzod = 30;
uint8_t filter_counter;
float filtr;
extern uint8_t lane_switching_move;



void StartCzujnikiTask(void const * argument) {
	while (1) {


		if (HAL_GPIO_ReadPin(CzujnikLewy_GPIO_Port, CzujnikLewy_Pin)
				== GPIO_PIN_RESET) {
			flags[0] = 1;
		} else {
			flags[0] = 0;
		}

		if (HAL_GPIO_ReadPin(CzujnikPrawy_GPIO_Port, CzujnikPrawy_Pin)
				== GPIO_PIN_RESET) {
			flags[1] = 1;
		} else{
			flags[1] = 0;
		}


		///filtr do zrobienia!
		filtr = 0.891 * filtr + 0.109*adc_raw[2];

		if(filtr > 500)
			filter_counter++;
		else
			filter_counter=0;

		if(filter_counter > 250)
			filter_counter = 10;


			if(filter_counter > 10 && lane_switching_move == 0 && autonomous_task != parking){
				flags[2]=1;
				sidesignals=SIDETURN_LEFT;
				autonomous_task = laneswitch;
				lane_switching_move = 1;
				HAL_GPIO_WritePin(Change_Lane_GPIO_Port,Change_Lane_Pin, GPIO_PIN_SET);}
			else{
				flags[2]=0;}

	osDelay(10);
}
}

//void StartCzujnikiTask(void const * argument) {
////	vSemaphoreCreateBinary(i2c2_semaphore);
////	osSemaphoreWait(i2c2_semaphore, osWaitForever);
////	MX_I2C2_Init();
////
////	lane_change_treshold = 600;
////
////	static uint8_t expander_bits[5];
////	expander_bits[0] = 0x00;
////	HAL_I2C_Master_Transmit(&hi2c2, EXPANDER_ADRESS, expander_bits, 1, 5);
////	osDelay(10);
////
////	for (int sensor = 0; sensor < VLX_SENSOR_COUNT; sensor++) {
////
////		expander_bits[0] |= (1 << sensor);
////		HAL_I2C_Master_Transmit(&hi2c2, EXPANDER_ADRESS, expander_bits, 1, 5);
////		VLX_CURRENT_ADRESS = ADDRESS_DEFAULT + sensor*2 + 10;
////		osDelay(5);
////		VL53L0X_init();
////		osDelay(100);
////		VL53L0X_startContinuous(0);
////		osDelay(100);
////	}
//	while (1) {
////		for (int sensor = 0; sensor < VLX_SENSOR_COUNT; sensor++) {
////			VLX_CURRENT_ADRESS = ADDRESS_DEFAULT + sensor*2 + 10;
////			if(sensor == 0 || sensor == 1 || sensor == 5 || sensor == 6 || sensor == 7)
////			range[sensor] = readRangeContinuousMillimeters();
////			osDelay(10);
////		}
////
////		//if(driving_state == autonomous && range[7] < (lane_change_treshold + 100))
////		if(range[7] < (lane_change_treshold + 100))
//		//{
////			autonomous_task = laneswitch;
//		//HAL_GPIO_WritePin(Change_Lane_GPIO_Port,Change_Lane_Pin, GPIO_PIN_SET);
//		//}
//		osDelay(500);
//	}
//}


