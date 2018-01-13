/*
 * Futaba.c
 *
 *  Created on: 13.01.2018
 *      Author: mice
 */


#include "Futaba.h"

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#include "usart.h"
#include "tim.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

extern osSemaphoreId DriveControlSemaphoreHandle;

uint8_t FutabaBuffer[25];
uint8_t FutabaStatus = RX_FRAME_FAILSAFE;

#define RX_OFFSET_T 192
#define RX_OFFSET_AER 992

#define SBUS_TIME_NEEDED_PER_FRAME 3000


#define SBUS_MAX_CHANNEL 18
#define SBUS_FRAME_SIZE 25

#define SBUS_FRAME_BEGIN_BYTE 0x0F

#define SBUS_DIGITAL_CHANNEL_MIN 172
#define SBUS_DIGITAL_CHANNEL_MAX 1812

static bool sbusFrameDone = false;
uint16_t FutabaChannelData[SBUS_MAX_CHANNEL];

const char rcChannelLetters[] = "TAER12345678abcdefgh";

float rcData[SBUS_MAX_CHANNEL];     // interval [1000;2000]

#define SBUS_FLAG_CHANNEL_17        (1 << 0)
#define SBUS_FLAG_CHANNEL_18        (1 << 1)
#define SBUS_FLAG_SIGNAL_LOSS       (1 << 2)
#define SBUS_FLAG_FAILSAFE_ACTIVE   (1 << 3)

struct sbusFrame_s {
    uint8_t syncByte;
    unsigned int chan0 : 11;
    unsigned int chan1 : 11;
    unsigned int chan2 : 11;
    unsigned int chan3 : 11;
    unsigned int chan4 : 11;
    unsigned int chan5 : 11;
    unsigned int chan6 : 11;
    unsigned int chan7 : 11;
    unsigned int chan8 : 11;
    unsigned int chan9 : 11;
    unsigned int chan10 : 11;
    unsigned int chan11 : 11;
    unsigned int chan12 : 11;
    unsigned int chan13 : 11;
    unsigned int chan14 : 11;
    unsigned int chan15 : 11;
    uint8_t flags;
    uint8_t endByte;
} __attribute__ ((__packed__));

typedef union {
    uint8_t bytes[SBUS_FRAME_SIZE];
    struct sbusFrame_s frame;
} sbusFrame_t;

static sbusFrame_t sbusFrame;

void StartFutabaTask(void const * argument)
{
	MX_USART1_UART_Init();
	MX_TIM14_Init();
	HAL_TIM_Base_Start(&htim14);
	HAL_UART_Receive_DMA(&huart1, FutabaBuffer, 1);

	while(1)
	{
		osDelay(1000);
	}
}
void FutabaRx_Irq(void) {
	sbusDataReceive(FutabaBuffer[0]);
	FutabaStatus = sbusFrameStatus();
	if(FutabaStatus == RX_FRAME_COMPLETE)
		osSemaphoreRelease(DriveControlSemaphoreHandle);
}
void sbusDataReceive(uint16_t c)
{
    static uint8_t sbusFramePosition = 0;
    static int16_t sbusFrameStartAt = 0;
    int16_t now = GetMicroTick();

    int16_t sbusFrameTime = now - sbusFrameStartAt;

    if (sbusFrameTime > (SBUS_TIME_NEEDED_PER_FRAME + 500)) {
        sbusFramePosition = 0;
    }

    if (sbusFramePosition == 0) {
        if (c != SBUS_FRAME_BEGIN_BYTE) {
            return;
        }
        sbusFrameStartAt = now;
    }

    if (sbusFramePosition < SBUS_FRAME_SIZE) {
        sbusFrame.bytes[sbusFramePosition++] = (uint8_t)c;
        if (sbusFramePosition < SBUS_FRAME_SIZE) {
            sbusFrameDone = false;
        } else {
            sbusFrameDone = true;
        }
    }
}

uint8_t sbusFrameStatus(void)
{
    if (!sbusFrameDone) {
        return RX_FRAME_PENDING;
    }
    sbusFrameDone = false;

    FutabaChannelData[0] = sbusFrame.frame.chan0;
    FutabaChannelData[1] = sbusFrame.frame.chan1;
    FutabaChannelData[2] = sbusFrame.frame.chan2;
    FutabaChannelData[3] = sbusFrame.frame.chan3;
    FutabaChannelData[4] = sbusFrame.frame.chan4;
    FutabaChannelData[5] = sbusFrame.frame.chan5;
    FutabaChannelData[6] = sbusFrame.frame.chan6;
    FutabaChannelData[7] = sbusFrame.frame.chan7;
    FutabaChannelData[8] = sbusFrame.frame.chan8;
    FutabaChannelData[9] = sbusFrame.frame.chan9;
    FutabaChannelData[10] = sbusFrame.frame.chan10;
    FutabaChannelData[11] = sbusFrame.frame.chan11;
    FutabaChannelData[12] = sbusFrame.frame.chan12;
    FutabaChannelData[13] = sbusFrame.frame.chan13;
    FutabaChannelData[14] = sbusFrame.frame.chan14;
    FutabaChannelData[15] = sbusFrame.frame.chan15;

    if (sbusFrame.frame.flags & SBUS_FLAG_CHANNEL_17) {
        FutabaChannelData[16] = SBUS_DIGITAL_CHANNEL_MAX;
    } else {
        FutabaChannelData[16] = SBUS_DIGITAL_CHANNEL_MIN;
    }

    if (sbusFrame.frame.flags & SBUS_FLAG_CHANNEL_18) {
        FutabaChannelData[17] = SBUS_DIGITAL_CHANNEL_MAX;
    } else {
        FutabaChannelData[17] = SBUS_DIGITAL_CHANNEL_MIN;
    }

    if (sbusFrame.frame.flags & SBUS_FLAG_SIGNAL_LOSS) {

    }
    if (sbusFrame.frame.flags & SBUS_FLAG_FAILSAFE_ACTIVE) {
        // internal failsafe enabled and rx failsafe flag set

        // RX *should* still be sending valid channel data, so use it.
        return RX_FRAME_COMPLETE | RX_FRAME_FAILSAFE;
    }
    return RX_FRAME_COMPLETE;
}
int16_t GetMicroTick(void) {
	int16_t tick = TIM14->CNT;
	return tick;
}
void MicroDelay(uint16_t Delay) {
	if(Delay > 0x7fff)
		return;
	int16_t tickstart = GetMicroTick();
	while ((GetMicroTick() - tickstart) < Delay) {
	}
}


