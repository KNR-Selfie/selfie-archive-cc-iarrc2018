/*
 * filtering.h
 *
 *  Created on: Nov 18, 2016
 *      Author: mice
 */

#ifndef FILTERING_H_
#define FILTERING_H_

#include "stm32f7xx_hal.h"

/* Wska�nik filtru 2-go rz�du dla Butterwortha */
#define BUTTERWORTH_Q 1.0f / sqrtf(2.0f)
/* Szerokosc pasma filtru �rodkowozaporowego */
#define BIQUAD_BANDWIDTH 1.9f

#define M_PI_FLOAT  3.14159265358979323846f

/* Struktura i Funkcja filtru 2-go rz�du */
typedef struct filter_s {
    float b0, b1, b2, a1, a2;
    float d1, d2;
} filter_t;

typedef struct pt1Filter_s {
    float state;
    float k;
    float RC;
    float dT;
} pt1Filter_t;

typedef float (*filterApplyFnPtr)(void *filter, float input);

float filter_apply(filter_t *filter, float input);

/* Filtr Lowpass */
void lpf_filter_init(filter_t *filter, float filterFreq, uint32_t refreshRate);

/* Filtr �rodkowozaporowy */
float notch_filter_qualityfactor(uint16_t centerFreq, uint16_t cutoff);
void notch_filter_init(filter_t *filter, float filterFreq, uint32_t refreshRate, float Q);

void pt1FilterInit(pt1Filter_t *filter, uint8_t f_cut, float dT);
float pt1FilterApply(pt1Filter_t *filter, float input);

float nullFilterApply(void *filter, float input);

#endif /* FILTERING_H_ */
