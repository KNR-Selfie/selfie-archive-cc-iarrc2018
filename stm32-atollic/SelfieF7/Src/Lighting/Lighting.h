/*
 * Lighting.h
 *
 *  Created on: 14.11.2017
 *      Author: mice
 */

#ifndef APPLICATION_USER_LIGHTING_LIGHTING_H_
#define APPLICATION_USER_LIGHTING_LIGHTING_H_

#include "math.h"
#include "cmsis_os.h"

void StartLightingTask(void const * argument);
void ws2812_init(void);
void ws2812_set_color(int led, uint8_t r, uint8_t g, uint8_t b);
void ws2812_update_color(void);
void hsi2rgb(float H, float S, float I, int* rgb);

void RXtoLighting(const uint16_t* rx_data);

typedef enum {
	SIDETURN_NONE = 0,
    SIDETURN_LEFT,
	SIDETURN_RIGHT,
	SIDETURN_EMERGENCY
} SideTurnSignalType_e;
typedef enum {
	BRAKE_NONE = 0,
    BRAKE_NORMAL,
	BRAKE_EMERGENCY,
} BrakeSignalType_e;

extern SideTurnSignalType_e sidesignals;
extern BrakeSignalType_e brakesignals;
#endif /* APPLICATION_USER_LIGHTING_LIGHTING_H_ */
