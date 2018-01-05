/*
 * Gyro.h
 *
 *  Created on: 21.11.2017
 *      Author: mice
 */

#ifndef APPLICATION_USER_GYRO_GYRO_H_
#define APPLICATION_USER_GYRO_GYRO_H_


extern void StartGyroTask(void const * argument);
extern void mpuInit2(void);

extern float CumulativeYaw;
extern float YawRate;

extern float temperature;
#endif /* APPLICATION_USER_GYRO_GYRO_H_ */
