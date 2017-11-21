/*
 * Lighting.c
 *
 *  Created on: 12.11.2017
 *      Author: mice
 */
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "math.h"

#include "tim.h"

#define H_VAL 91
#define L_VAL 47
#define N_LEDS 22
#define BITS_PER_LED 3*8
#define BIT_BUF_SIZE N_LEDS*BITS_PER_LED+81
#define NLED_SIZE N_LEDS*BITS_PER_LED


uint16_t ws2812BitBuf[BIT_BUF_SIZE];

void StartLightingTask(void const * argument)
{

}
void ws2812_init(void) {
    MX_TIM4_Init();
    memset(ws2812BitBuf, 0, BIT_BUF_SIZE);
    for(uint16_t i=0; i<NLED_SIZE; i++) ws2812BitBuf[i]=L_VAL;
    HAL_TIM_PWM_Stop_DMA(&htim4, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start_DMA(&htim4, TIM_CHANNEL_3, (uint32_t *) ws2812BitBuf, BIT_BUF_SIZE);
}

void ws2812_set_color(int led, uint8_t r, uint8_t g, uint8_t b) {
    int i = led * BITS_PER_LED;
    uint8_t mask;
    mask = 0x80;
    while (mask) {
        ws2812BitBuf[i] = (mask & g) ? H_VAL : L_VAL;
        mask >>= 1;
        i++;
    }
    mask = 0x80;
    while (mask) {
        ws2812BitBuf[i] = (mask & r) ? H_VAL : L_VAL;
        mask >>= 1;
        i++;
    }
    mask = 0x80;
    while (mask) {
        ws2812BitBuf[i] = (mask & b) ? H_VAL : L_VAL;
        mask >>= 1;
        i++;
    }
}
void ws2812_update_color(void)
{
    for(uint16_t i=0; i<NLED_SIZE; i++) ws2812BitBuf[i]=L_VAL;
}
void hsi2rgb(float H, float S, float I, int* rgb)
{
  int r, g, b;
  H = fmod(H,360.0f); // cycle H around to 0-360 degrees
  H = 3.14159f*H/(float)180.0f; // Convert to radians.
  S = S>0?(S<1?S:1):0; // clamp S and I to interval [0,1]
  I = I>0?(I<1?I:1):0;

  // Math! Thanks in part to Kyle Miller.
  if(H < 2.09439f) {
    r = 150.0f*I/3*(1.0f+S*cosf(H)/cosf(1.047196667f-H));
    g = 255.0f*I/3*(1.0f+S*(1.0f-cosf(H)/cosf(1.047196667f-H)));
    b = 255.0f*I/3*(1.0f-S);
  } else if(H < 4.188787f) {
    H = H - 2.09439f;
    g = 255.0f*I/3.0f*(1.0f+S*cosf(H)/cosf(1.047196667f-H));
    b = 255.0f*I/3.0f*(1.0f+S*(1.0f-cosf(H)/cosf(1.047196667f-H)));
    r = 150.0f*I/3.0f*(1.0f-S);
  } else {
    H = H - 4.188787f;
    b = 255.0f*I/3.0f*(1.0f+S*cosf(H)/cosf(1.047196667f-H));
    r = 150.0f*I/3.0f*(1.0f+S*(1.0f-cosf(H)/cosf(1.047196667f-H)));
    g = 255.0f*I/3.0f*(1.0f-S);
  }
  rgb[0]=r;
  rgb[1]=g;
  rgb[2]=b;
}
