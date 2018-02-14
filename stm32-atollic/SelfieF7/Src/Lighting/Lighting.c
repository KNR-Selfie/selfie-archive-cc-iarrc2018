/*
 * Lighting.c
 *
 *  Created on: 12.11.2017
 *      Author: mice
 */
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "Governor.h"
#include "Lighting.h"
#include "Futaba.h"

#include "cmsis_os.h"
#include "math.h"
#include "tim.h"

#define H_VAL 91
#define L_VAL 47
#define N_LEDS 23
#define BITS_PER_LED 3*8
#define BIT_BUF_SIZE N_LEDS*BITS_PER_LED+81
#define NLED_SIZE N_LEDS*BITS_PER_LED


uint16_t ws2812BitBuf[BIT_BUF_SIZE];

uint16_t RX_AETR[4];
uint16_t RX_SW;
uint16_t RX_POT;

int rgb[22][3];
float brightness;

SideTurnSignalType_e sidesignals = SIDETURN_EMERGENCY;
BrakeSignalType_e brakesignals = BRAKE_NONE;

void StartLightingTask(void const * argument) {
	ws2812_init();
	while (1) {
		RXtoLighting(FutabaChannelData);
		if (RX_POT > 1027) {
			brightness = (RX_POT - 1027.f) / 920.f;
			static int16_t scounter1 = 1;
			scounter1++;
			if (scounter1 > 359)
				scounter1 = 0;

			for (int indeks = 0; indeks < 22; indeks++) {
				if ((scounter1 + 5 * indeks) > 359)
					hsi2rgb((scounter1 + 5 * indeks - 359), 1, 1,
							&rgb[indeks][0]);
				else
					hsi2rgb((scounter1 + 5 * indeks), 1, 1, &rgb[indeks][0]);
				ws2812_set_color(indeks, rgb[indeks][0], rgb[indeks][1],
						rgb[indeks][2]);
			}
		} else {
			brightness = -(RX_POT - 1027.f) / 920.f;
			ws2812_set_color(4, 0, 0, 0);
			ws2812_set_color(5, 0, 0, 0);
			ws2812_set_color(6, 0, 0, 0);

			ws2812_set_color(15, 0, 0, 0);
			ws2812_set_color(16, 0, 0, 0);
			ws2812_set_color(17, 0, 0, 0);
			if ( sidesignals == SIDETURN_EMERGENCY) {
				static int cnt_ind0 = 0;
				++cnt_ind0;
				if (cnt_ind0 > 50) {
					cnt_ind0 = 0;
				} else if (cnt_ind0> 30) {

					ws2812_set_color(20, 255, 120, 0);
					ws2812_set_color(21, 255, 120, 0);
					ws2812_set_color(1, 255, 120, 0);
					ws2812_set_color(0, 255, 120, 0);

					ws2812_set_color(9, 255, 120, 0);
					ws2812_set_color(10, 255, 120, 0);
					ws2812_set_color(12, 255, 120, 0);
					ws2812_set_color(11, 255, 120, 0);

				} else if (cnt_ind0> 25) {
					ws2812_set_color(18, 255, 120, 0);
					ws2812_set_color(19, 255, 120, 0);
					ws2812_set_color(3, 255, 120, 0);
					ws2812_set_color(2, 255, 120, 0);

					ws2812_set_color(7, 255, 120, 0);
					ws2812_set_color(8, 255, 120, 0);
					ws2812_set_color(14, 255, 120, 0);
					ws2812_set_color(13, 255, 120, 0);


				} else {
					ws2812_set_color(3, 0, 0, 0);
					ws2812_set_color(2, 0, 0, 0);
					ws2812_set_color(1, 0, 0, 0);
					ws2812_set_color(0, 0, 0, 0);

					ws2812_set_color(18, 0, 0, 0);
					ws2812_set_color(19, 0, 0, 0);
					ws2812_set_color(20, 0, 0, 0);
					ws2812_set_color(21, 0, 0, 0);

					ws2812_set_color(7, 0, 0, 0);
					ws2812_set_color(8, 0, 0, 0);
					ws2812_set_color(9, 0, 0, 0);
					ws2812_set_color(10, 0, 0, 0);

					ws2812_set_color(14, 0, 0, 0);
					ws2812_set_color(13, 0, 0, 0);
					ws2812_set_color(12, 0, 0, 0);
					ws2812_set_color(11, 0, 0, 0);
				}
			}
			else if (sidesignals == SIDETURN_LEFT) {
				static int cnt_ind1 = 0;
				++cnt_ind1;
				if (cnt_ind1 > 100) {
					cnt_ind1 = 0;
					ws2812_set_color(7, 0, 0, 0);
					ws2812_set_color(8, 0, 0, 0);
					ws2812_set_color(9, 0, 0, 0);
					ws2812_set_color(10, 0, 0, 0);

					ws2812_set_color(14, 0, 0, 0);
					ws2812_set_color(13, 0, 0, 0);
					ws2812_set_color(12, 0, 0, 0);
					ws2812_set_color(11, 0, 0, 0);
				} else if (cnt_ind1 > 60) {
					ws2812_set_color(7, 0, 0, 0);
					ws2812_set_color(8, 0, 0, 0);
					ws2812_set_color(9, 0, 0, 0);
					ws2812_set_color(10, 0, 0, 0);

					ws2812_set_color(14, 0, 0, 0);
					ws2812_set_color(13, 0, 0, 0);
					ws2812_set_color(12, 0, 0, 0);
					ws2812_set_color(11, 0, 0, 0);
				} else if (cnt_ind1 > 40) {
					ws2812_set_color(7, 0, 0, 0);
					ws2812_set_color(8, 0, 0, 0);
					ws2812_set_color(9, 0, 0, 0);
					ws2812_set_color(10, 255, 120, 0);

					ws2812_set_color(14, 0, 0, 0);
					ws2812_set_color(13, 0, 0, 0);
					ws2812_set_color(12, 0, 0, 0);
					ws2812_set_color(11, 255, 120, 0);
				} else if (cnt_ind1 > 30) {
					ws2812_set_color(7, 0, 0, 0);
					ws2812_set_color(8, 0, 0, 0);
					ws2812_set_color(9, 255, 120, 0);
					ws2812_set_color(10, 0, 0, 0);

					ws2812_set_color(14, 0, 0, 0);
					ws2812_set_color(13, 0, 0, 0);
					ws2812_set_color(12, 255, 120, 0);
					ws2812_set_color(11, 0, 0, 0);
				} else if (cnt_ind1 > 20) {
					ws2812_set_color(7, 0, 0, 0);
					ws2812_set_color(8, 255, 120, 0);
					ws2812_set_color(9, 0, 0, 0);
					ws2812_set_color(10, 0, 0, 0);

					ws2812_set_color(14, 0, 0, 0);
					ws2812_set_color(13, 255, 120, 0);
					ws2812_set_color(12, 0, 0, 0);
					ws2812_set_color(11, 0, 0, 0);
				} else if (cnt_ind1 > 10) {
					ws2812_set_color(7, 255, 120, 0);
					ws2812_set_color(8, 0, 0, 0);
					ws2812_set_color(9, 0, 0, 0);
					ws2812_set_color(10, 0, 0, 0);

					ws2812_set_color(14, 255, 120, 0);
					ws2812_set_color(13, 0, 0, 0);
					ws2812_set_color(12, 0, 0, 0);
					ws2812_set_color(11, 0, 0, 0);
				}

				ws2812_set_color(0, 0, 0, 0);
				ws2812_set_color(21, 0, 0, 0);

				ws2812_set_color(1, 255, 255, 255);
				ws2812_set_color(2, 255, 255, 255);
				ws2812_set_color(3, 255, 255, 255);
				if (brakesignals == BRAKE_NORMAL) {
					ws2812_set_color(20, 255, 0, 0);
					ws2812_set_color(19, 255, 0, 0);
					ws2812_set_color(18, 255, 0, 0);

					ws2812_set_color(16, 255, 0, 0);

				} else {
					ws2812_set_color(20, 50, 0, 0);
					ws2812_set_color(19, 50, 0, 0);
					ws2812_set_color(18, 50, 0, 0);

					ws2812_set_color(16, 0, 0, 0);
				}
			} else if (sidesignals == SIDETURN_RIGHT) {
				static int cnt_ind2 = 0;
				++cnt_ind2;
				if (cnt_ind2 > 100) {
					cnt_ind2 = 0;
					ws2812_set_color(3, 0, 0, 0);
					ws2812_set_color(2, 0, 0, 0);
					ws2812_set_color(1, 0, 0, 0);
					ws2812_set_color(0, 0, 0, 0);

					ws2812_set_color(18, 0, 0, 0);
					ws2812_set_color(19, 0, 0, 0);
					ws2812_set_color(20, 0, 0, 0);
					ws2812_set_color(21, 0, 0, 0);
				} else if (cnt_ind2 > 60) {
					ws2812_set_color(3, 0, 0, 0);
					ws2812_set_color(2, 0, 0, 0);
					ws2812_set_color(1, 0, 0, 0);
					ws2812_set_color(0, 0, 0, 0);

					ws2812_set_color(18, 0, 0, 0);
					ws2812_set_color(19, 0, 0, 0);
					ws2812_set_color(20, 0, 0, 0);
					ws2812_set_color(21, 0, 0, 0);
				} else if (cnt_ind2 > 40) {
					ws2812_set_color(3, 0, 0, 0);
					ws2812_set_color(2, 0, 0, 0);
					ws2812_set_color(1, 0, 0, 0);
					ws2812_set_color(0, 255, 120, 0);

					ws2812_set_color(18, 0, 0, 0);
					ws2812_set_color(19, 0, 0, 0);
					ws2812_set_color(20, 0, 0, 0);
					ws2812_set_color(21, 255, 120, 0);
				} else if (cnt_ind2 > 30) {
					ws2812_set_color(3, 0, 0, 0);
					ws2812_set_color(2, 0, 0, 0);
					ws2812_set_color(1, 255, 120, 0);
					ws2812_set_color(0, 0, 0, 0);

					ws2812_set_color(18, 0, 0, 0);
					ws2812_set_color(19, 0, 0, 0);
					ws2812_set_color(20, 255, 120, 0);
					ws2812_set_color(21, 0, 0, 0);
				} else if (cnt_ind2 > 20) {
					ws2812_set_color(3, 0, 0, 0);
					ws2812_set_color(2, 255, 120, 0);
					ws2812_set_color(1, 0, 0, 0);
					ws2812_set_color(0, 0, 0, 0);

					ws2812_set_color(18, 0, 0, 0);
					ws2812_set_color(19, 255, 120, 0);
					ws2812_set_color(20, 0, 0, 0);
					ws2812_set_color(21, 0, 0, 0);
				} else if (cnt_ind2 > 10) {
					ws2812_set_color(3, 255, 120, 0);
					ws2812_set_color(2, 0, 0, 0);
					ws2812_set_color(1, 0, 0, 0);
					ws2812_set_color(0, 0, 0, 0);

					ws2812_set_color(18, 255, 120, 0);
					ws2812_set_color(19, 0, 0, 0);
					ws2812_set_color(20, 0, 0, 0);
					ws2812_set_color(21, 0, 0, 0);
				}

				ws2812_set_color(11, 0, 0, 0);
				ws2812_set_color(10, 0, 0, 0);

				ws2812_set_color(7, 255, 255, 255);
				ws2812_set_color(8, 255, 255, 255);
				ws2812_set_color(9, 255, 255, 255);
				if (brakesignals == BRAKE_NORMAL) {
					ws2812_set_color(12, 255, 0, 0);
					ws2812_set_color(13, 255, 0, 0);
					ws2812_set_color(14, 255, 0, 0);

					ws2812_set_color(16, 255, 0, 0);

				} else {
					ws2812_set_color(12, 50, 0, 0);
					ws2812_set_color(13, 50, 0, 0);
					ws2812_set_color(14, 50, 0, 0);

					ws2812_set_color(16, 0, 0, 0);
				}

			} else {

				ws2812_set_color(1, 255, 255, 255);
				ws2812_set_color(2, 255, 255, 255);
				ws2812_set_color(3, 255, 255, 255);

				ws2812_set_color(7, 255, 255, 255);
				ws2812_set_color(8, 255, 255, 255);
				ws2812_set_color(9, 255, 255, 255);

				ws2812_set_color(0, 0, 0, 0);
				ws2812_set_color(21, 0, 0, 0);
				ws2812_set_color(11, 0, 0, 0);
				ws2812_set_color(10, 0, 0, 0);
				if (brakesignals == BRAKE_NORMAL) {
					ws2812_set_color(12, 255, 0, 0);
					ws2812_set_color(13, 255, 0, 0);
					ws2812_set_color(14, 255, 0, 0);

					ws2812_set_color(16, 255, 0, 0);

					ws2812_set_color(20, 255, 0, 0);
					ws2812_set_color(19, 255, 0, 0);
					ws2812_set_color(18, 255, 0, 0);

				} else {
					ws2812_set_color(12, 50, 0, 0);
					ws2812_set_color(13, 50, 0, 0);
					ws2812_set_color(14, 50, 0, 0);

					ws2812_set_color(16, 0, 0, 0);

					ws2812_set_color(20, 50, 0, 0);
					ws2812_set_color(19, 50, 0, 0);
					ws2812_set_color(18, 50, 0, 0);
				}
			}

		}

		// TOP LED
		if (RX_SW < 500) //gorna pozycja przelacznika - pelna kontrola
				{
			static int cnt = 0;
			++cnt;
			if (cnt > 100) {
				cnt = 0;
			} else if (cnt > 50) {
				ws2812_set_color(22, 0, 0, 255);
			} else
				ws2812_set_color(22, 0, 0, 0);
		} else {
			ws2812_set_color(22, 0, 0, 0);
		}
		if (FutabaStatus == RX_FRAME_FAILSAFE)
			sidesignals = SIDETURN_EMERGENCY;
		else if (autonomous_task != parking && sidesignals == SIDETURN_EMERGENCY) {
			sidesignals = SIDETURN_NONE;
		}
		osDelay(10);
	}
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
    r=(uint8_t)(r*brightness);
    g=(uint8_t)(g*brightness);
    b=(uint8_t)(b*brightness);
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
void RXtoLighting (const uint16_t* rx_data)
{
    for (int stick = 0; stick < 4; stick++)
        RX_AETR[stick] = rx_data[stick];
    RX_SW = rx_data[5];
    RX_POT = rx_data[4];

}
