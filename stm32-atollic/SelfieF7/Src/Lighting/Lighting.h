/*
 * Lighting.h
 *
 *  Created on: 14.11.2017
 *      Author: mice
 */

#ifndef APPLICATION_USER_LIGHTING_LIGHTING_H_
#define APPLICATION_USER_LIGHTING_LIGHTING_H_

#include "math.h"

void StartLightingTask(void const * argument);
void ws2812_init(void);
void ws2812_set_color(int led, uint8_t r, uint8_t g, uint8_t b);
void ws2812_update_color(void);
void hsi2rgb(float H, float S, float I, int* rgb);

extern uint16_t a_channels[16];
void RXtoLighting(const uint16_t* rx_data);

#endif /* APPLICATION_USER_LIGHTING_LIGHTING_H_ */
