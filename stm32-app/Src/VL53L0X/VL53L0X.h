#ifndef VL53L0X_h
#define VL53L0X_h

#include "stm32f7xx_hal.h"
#include <stdbool.h>

#define VL53L0X_I2C_TIMEOUT	10 // milliseconds

    enum VL53L0X_regAddr
    {
      SYSRANGE_START                              = 0x00,

      SYSTEM_THRESH_HIGH                          = 0x0C,
      SYSTEM_THRESH_LOW                           = 0x0E,

      SYSTEM_SEQUENCE_CONFIG                      = 0x01,
      SYSTEM_RANGE_CONFIG                         = 0x09,
      SYSTEM_INTERMEASUREMENT_PERIOD              = 0x04,

      SYSTEM_INTERRUPT_CONFIG_GPIO                = 0x0A,

      GPIO_HV_MUX_ACTIVE_HIGH                     = 0x84,

      SYSTEM_INTERRUPT_CLEAR                      = 0x0B,

      RESULT_INTERRUPT_STATUS                     = 0x13,
      RESULT_RANGE_STATUS                         = 0x14,

      RESULT_CORE_AMBIENT_WINDOW_EVENTS_RTN       = 0xBC,
      RESULT_CORE_RANGING_TOTAL_EVENTS_RTN        = 0xC0,
      RESULT_CORE_AMBIENT_WINDOW_EVENTS_REF       = 0xD0,
      RESULT_CORE_RANGING_TOTAL_EVENTS_REF        = 0xD4,
      RESULT_PEAK_SIGNAL_RATE_REF                 = 0xB6,

      ALGO_PART_TO_PART_RANGE_OFFSET_MM           = 0x28,

      I2C_SLAVE_DEVICE_ADDRESS                    = 0x8A,

      MSRC_CONFIG_CONTROL                         = 0x60,

      PRE_RANGE_CONFIG_MIN_SNR                    = 0x27,
      PRE_RANGE_CONFIG_VALID_PHASE_LOW            = 0x56,
      PRE_RANGE_CONFIG_VALID_PHASE_HIGH           = 0x57,
      PRE_RANGE_MIN_COUNT_RATE_RTN_LIMIT          = 0x64,

      FINAL_RANGE_CONFIG_MIN_SNR                  = 0x67,
      FINAL_RANGE_CONFIG_VALID_PHASE_LOW          = 0x47,
      FINAL_RANGE_CONFIG_VALID_PHASE_HIGH         = 0x48,
      FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT = 0x44,

      PRE_RANGE_CONFIG_SIGMA_THRESH_HI            = 0x61,
      PRE_RANGE_CONFIG_SIGMA_THRESH_LO            = 0x62,

      PRE_RANGE_CONFIG_VCSEL_PERIOD               = 0x50,
      PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI          = 0x51,
      PRE_RANGE_CONFIG_TIMEOUT_MACROP_LO          = 0x52,

      SYSTEM_HISTOGRAM_BIN                        = 0x81,
      HISTOGRAM_CONFIG_INITIAL_PHASE_SELECT       = 0x33,
      HISTOGRAM_CONFIG_READOUT_CTRL               = 0x55,

      FINAL_RANGE_CONFIG_VCSEL_PERIOD             = 0x70,
      FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI        = 0x71,
      FINAL_RANGE_CONFIG_TIMEOUT_MACROP_LO        = 0x72,
      CROSSTALK_COMPENSATION_PEAK_RATE_MCPS       = 0x20,

      MSRC_CONFIG_TIMEOUT_MACROP                  = 0x46,

      SOFT_RESET_GO2_SOFT_RESET_N                 = 0xBF,
      IDENTIFICATION_MODEL_ID                     = 0xC0,
      IDENTIFICATION_REVISION_ID                  = 0xC2,

      OSC_CALIBRATE_VAL                           = 0xF8,

      GLOBAL_CONFIG_VCSEL_WIDTH                   = 0x32,
      GLOBAL_CONFIG_SPAD_ENABLES_REF_0            = 0xB0,
      GLOBAL_CONFIG_SPAD_ENABLES_REF_1            = 0xB1,
      GLOBAL_CONFIG_SPAD_ENABLES_REF_2            = 0xB2,
      GLOBAL_CONFIG_SPAD_ENABLES_REF_3            = 0xB3,
      GLOBAL_CONFIG_SPAD_ENABLES_REF_4            = 0xB4,
      GLOBAL_CONFIG_SPAD_ENABLES_REF_5            = 0xB5,

      GLOBAL_CONFIG_REF_EN_START_SELECT           = 0xB6,
      DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD         = 0x4E,
      DYNAMIC_SPAD_REF_EN_START_OFFSET            = 0x4F,
      POWER_MANAGEMENT_GO1_POWER_FORCE            = 0x80,

      VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV           = 0x89,

      ALGO_PHASECAL_LIM                           = 0x30,
      ALGO_PHASECAL_CONFIG_TIMEOUT                = 0x30,
    };

    typedef enum VL53L0X_vcselPeriodType { VL53L0X_VcselPeriodPreRange, VL53L0X_VcselPeriodFinalRange } VL53L0X_vcselPeriodType;

    extern uint8_t VL53L0X_last_status; // status of last I2C transmission
    extern I2C_HandleTypeDef hi2c2;

    void VL53L0X_setAddress(uint8_t new_addr);
    uint8_t VL53L0X_getAddress(void);

    bool VL53L0X_init(bool io_2v8); // default "io_2v8 = true"

    void VL53L0X_writeReg(uint8_t reg, uint8_t value);
    void VL53L0X_writeReg16Bit(uint8_t reg, uint16_t value);
    void VL53L0X_writeReg32Bit(uint8_t reg, uint32_t value);
    uint8_t VL53L0X_readReg(uint8_t reg);
    uint16_t VL53L0X_readReg16Bit(uint8_t reg);
    uint32_t VL53L0X_readReg32Bit(uint8_t reg);

    void VL53L0X_writeMulti(uint8_t reg, uint8_t const * src, uint8_t count);
    void VL53L0X_readMulti(uint8_t reg, uint8_t * dst, uint8_t count);

    bool VL53L0X_setSignalRateLimit(float limit_Mcps);
    float VL53L0X_getSignalRateLimit(void);

    bool VL53L0X_setMeasurementTimingBudget(uint32_t budget_us);
    uint32_t VL53L0X_getMeasurementTimingBudget(void);

    bool VL53L0X_setVcselPulsePeriod(VL53L0X_vcselPeriodType type, uint8_t period_pclks);
    uint8_t VL53L0X_getVcselPulsePeriod(VL53L0X_vcselPeriodType type);

    void VL53L0X_startContinuous(uint32_t period_ms); // default "period_ms = 0"
    void VL53L0X_stopContinuous(void);
    uint16_t VL53L0X_readRangeContinuousMillimeters(void);
    uint16_t VL53L0X_readRangeSingleMillimeters(void);
    void VL53L0X_setTimeout(uint16_t timeout);
    uint16_t VL53L0X_getTimeout(void);
    bool VL53L0X_timeoutOccurred(void);

#endif



