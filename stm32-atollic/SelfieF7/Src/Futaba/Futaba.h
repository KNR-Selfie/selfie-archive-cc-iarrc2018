/*
 * Futaba.h
 *
 *  Created on: 13.01.2018
 *      Author: mice
 */

#ifndef FUTABA_FUTABA_H_
#define FUTABA_FUTABA_H_

#include <stdint.h>

#define SBUS_MAX_CHANNEL 18

extern uint8_t FutabaStatus;
extern uint16_t FutabaChannelData[SBUS_MAX_CHANNEL];
extern float rcData[SBUS_MAX_CHANNEL];       // ( -1.0 , 1.0 ) or (0 - 1.0)

typedef enum {
    RX_FRAME_PENDING = 0,
    RX_FRAME_COMPLETE = (1 << 0),
    RX_FRAME_FAILSAFE = (1 << 1)
} rxFrameState_e;

typedef enum rc_alias {
    THROTTLE = 0,
    ROLL,
    PITCH,
    YAW,
    AUX1,
    AUX2,
    AUX3,
    AUX4,
    AUX5,
    AUX6,
    AUX7,
    AUX8
} rc_alias_e;

extern const char rcChannelLetters[];
void sbusDataReceive(uint16_t c);
uint8_t sbusFrameStatus(void);
void FutabaRx_Irq(void);
int16_t Get100Tick(void);
void Micro100Delay(uint16_t Delay);
#endif /* FUTABA_FUTABA_H_ */
