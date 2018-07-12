/*
 * filtering.c
 *
 *  Created on: Nov 18, 2016
 *      Author: mice
 */
#include "Filtering.h"
#include "main.h"
#include "stm32f7xx_hal.h"
#include "math.h"

float filter_apply(filter_t *filter, float input)
{
    const float result = filter->b0 * input + filter->d1;
    filter->d1 = filter->b1 * input - filter->a1 * result + filter->d2;
    filter->d2 = filter->b2 * input - filter->a2 * result;
    return result;
}
void lpf_filter_init(filter_t *filter, float filterFreq, uint32_t refreshRate) {
	const float Q = BUTTERWORTH_Q;
	const float sampleRate = refreshRate;
	const float omega = 2 * M_PI_FLOAT * filterFreq / sampleRate;
	const float sn = sinf(omega);
	const float cs = cosf(omega);
	const float alpha = sn / (2 * Q);

	float b0 = 0, b1 = 0, b2 = 0, a0 = 0, a1 = 0, a2 = 0;

	b0 = (1 - cs) / 2;
	b1 = 1 - cs;
	b2 = (1 - cs) / 2;
	a0 = 1 + alpha;
	a1 = -2 * cs;
	a2 = 1 - alpha;

	filter->b0 = b0 / a0;
	filter->b1 = b1 / a0;
	filter->b2 = b2 / a0;
	filter->a1 = a1 / a0;
	filter->a2 = a2 / a0;

	filter->d1 = filter->d2 = 0;
}
float notch_filter_qualityfactor(uint16_t centerFreq, uint16_t cutoff) {
    float octaves = log2f((float) centerFreq  / (float) cutoff) * 2;
    return sqrtf(powf(2, octaves)) / (powf(2, octaves) - 1);
}
void notch_filter_init(filter_t *filter, float filterFreq, uint32_t refreshRate, float Q)
{
	const float sampleRate = refreshRate;
	const float omega = 2 * M_PI_FLOAT * filterFreq / sampleRate;
	const float sn = sinf(omega);
	const float cs = cosf(omega);
	const float alpha = sn / (2 * Q);

	float b0 = 0, b1 = 0, b2 = 0, a0 = 0, a1 = 0, a2 = 0;

	b0 = 1;
	b1 = -2 * cs;
	b2 = 1;
	a0 = 1 + alpha;
	a1 = -2 * cs;
	a2 = 1 - alpha;

	filter->b0 = b0 / a0;
	filter->b1 = b1 / a0;
	filter->b2 = b2 / a0;
	filter->a1 = a1 / a0;
	filter->a2 = a2 / a0;

	filter->d1 = filter->d2 = 0;
}
void pt1FilterInit(pt1Filter_t *filter, uint8_t f_cut, float dT)
{
    filter->RC = 1.0f / ( 2.0f * M_PI_FLOAT * f_cut );
    filter->dT = dT;
    filter->k = filter->dT / (filter->RC + filter->dT);
}

float pt1FilterApply(pt1Filter_t *filter, float input)
{
    filter->state = filter->state + filter->k * (input - filter->state);
    return filter->state;
}
float nullFilterApply(void *filter, float input)
{
    UNUSED(filter);
    return input;
}
