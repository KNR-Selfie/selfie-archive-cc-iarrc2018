#include "VL53L0X.h"
#include "stdbool.h"
#include "stdint.h"
#include "cmsis_os.h"
#include "i2c.h"
#include "Czujniki.h"
extern osThreadId CzujnikiTaskHandle;
uint8_t vlxy_odczytywanie = 0;


uint16_t VLX_CURRENT_ADRESS = ADDRESS_DEFAULT + 10;
// Decode VCSEL (vertical cavity surface emitting laser) pulse period in PCLKs
// from register value
// based on VL53L0X_decode_vcsel_period()
#define decodeVcselPeriod(reg_val)      (((reg_val) + 1) << 1)

// Encode VCSEL pulse period register value from period in PCLKs
// based on VL53L0X_encode_vcsel_period()
#define encodeVcselPeriod(period_pclks) (((period_pclks) >> 1) - 1)

// Calculate macro period in *nanoseconds* from VCSEL period in PCLKs
// based on VL53L0X_calc_macro_period_ps()
// PLL_period_ps = 1655; macro_period_vclks = 2304
#define calcMacroPeriod(vcsel_period_pclks) ((((uint32_t)2304 * (vcsel_period_pclks) * 1655) + 500) / 1000)

uint8_t i2cRxBuffer[128], i2cTxBuffer[128];
uint8_t stop_variable;
uint8_t stop_variable; // read by init and used when starting measurement; is StopVariable field of VL53L0X_DevData_t structure in API
uint32_t measurement_timing_budget_us;

typedef struct SequenceStepEnables_s {
	bool tcc, msrc, dss, pre_range, final_range;
} SequenceStepEnables;

typedef struct SequenceStepTimeouts_s {
	uint16_t pre_range_vcsel_period_pclks, final_range_vcsel_period_pclks;

	uint16_t msrc_dss_tcc_mclks, pre_range_mclks, final_range_mclks;
	uint32_t msrc_dss_tcc_us, pre_range_us, final_range_us;
} SequenceStepTimeouts;

bool VL53L0X_init(void);
bool setSignalRateLimit(float limit_Mcps);
bool getSpadInfo(uint8_t * count, bool * type_is_aperture);
uint32_t getMeasurementTimingBudget(void);
void getSequenceStepEnables(SequenceStepEnables * enables);
void getSequenceStepTimeouts(SequenceStepEnables const * enables,
		SequenceStepTimeouts * timeouts);
uint16_t decodeTimeout(uint16_t reg_val);
uint16_t encodeTimeout(uint16_t timeout_mclks);
uint32_t timeoutMclksToMicroseconds(uint16_t timeout_period_mclks,
		uint8_t vcsel_period_pclks);
uint32_t timeoutMicrosecondsToMclks(uint32_t timeout_period_us,
		uint8_t vcsel_period_pclks);
uint8_t readReg(uint16_t adress, uint8_t reg);
uint16_t readReg16Bit(uint16_t adress, uint8_t reg);
uint16_t readReg16Bit_IT(uint16_t adress, uint8_t reg);
uint32_t readReg32Bit(uint16_t adress, uint8_t reg);
int writeReg(uint16_t adress, uint8_t reg, uint8_t value);
int writeReg_IT(uint16_t adress, uint8_t reg, uint8_t value);
void writeReg16Bit(uint16_t adress, uint8_t reg, uint16_t value);
void writeReg32Bit(uint16_t adress, uint8_t reg, uint32_t value);

bool setMeasurementTimingBudget(uint32_t budget_us);
bool performSingleRefCalibration(uint8_t vhv_init_byte);

uint16_t readRangeSingleMillimeters(void);

bool VL53L0X_init(void) {
	// ZMIANA ADRESU Z DEFAULT NA CURRENT
	if(writeReg(ADDRESS_DEFAULT, I2C_SLAVE_DEVICE_ADDRESS, VLX_CURRENT_ADRESS >> 1) != HAL_OK)
		return false;
	osDelay(10);
	// VL53L0X_DataInit() begin

	// sensor uses 1V8 mode for I/O by default; switch to 2V8 mode if necessary
	writeReg(VLX_CURRENT_ADRESS, VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV,
			readReg(VLX_CURRENT_ADRESS, VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV)
					| 0x01); // set bit 0

	// "Set I2C standard mode"
	writeReg(VLX_CURRENT_ADRESS, 0x88, 0x00);

	writeReg(VLX_CURRENT_ADRESS, 0x80, 0x01);
	writeReg(VLX_CURRENT_ADRESS, 0xFF, 0x01);
	writeReg(VLX_CURRENT_ADRESS, 0x00, 0x00);
	stop_variable = readReg(VLX_CURRENT_ADRESS, 0x91);
	writeReg(VLX_CURRENT_ADRESS, 0x00, 0x01);
	writeReg(VLX_CURRENT_ADRESS, 0xFF, 0x00);
	writeReg(VLX_CURRENT_ADRESS, 0x80, 0x00);

	// disable SIGNAL_RATE_MSRC (bit 1) and SIGNAL_RATE_PRE_RANGE (bit 4) limit checks
	writeReg(VLX_CURRENT_ADRESS, MSRC_CONFIG_CONTROL,
			readReg(VLX_CURRENT_ADRESS, MSRC_CONFIG_CONTROL) | 0x12);

	// set final range signal rate limit to 0.25 MCPS (million counts per second)
	setSignalRateLimit(0.25);

	writeReg(VLX_CURRENT_ADRESS, SYSTEM_SEQUENCE_CONFIG, 0xFF);

	// VL53L0X_DataInit() end

	// VL53L0X_StaticInit() begin

	uint8_t spad_count;
	bool spad_type_is_aperture;
	if (!getSpadInfo(&spad_count, &spad_type_is_aperture)) {
		return false;
	}

	// The SPAD map (RefGoodSpadMap) is read by VL53L0X_get_info_from_device() in
	// the API, but the same data seems to be more easily readable from
	// GLOBAL_CONFIG_SPAD_ENABLES_REF_0 through _6, so read it from there
	uint8_t ref_spad_map[6];
	HAL_I2C_Mem_Read(&hi2c2, VLX_CURRENT_ADRESS,
			GLOBAL_CONFIG_SPAD_ENABLES_REF_0, I2C_MEMADD_SIZE_8BIT,
			ref_spad_map, 6, 5);

	// -- VL53L0X_set_reference_spads() begin (assume NVM values are valid)

	writeReg(VLX_CURRENT_ADRESS, 0xFF, 0x01);
	writeReg(VLX_CURRENT_ADRESS, DYNAMIC_SPAD_REF_EN_START_OFFSET, 0x00);
	writeReg(VLX_CURRENT_ADRESS, DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD, 0x2C);
	writeReg(VLX_CURRENT_ADRESS, 0xFF, 0x00);
	writeReg(VLX_CURRENT_ADRESS, GLOBAL_CONFIG_REF_EN_START_SELECT, 0xB4);

	uint8_t first_spad_to_enable = spad_type_is_aperture ? 12 : 0; // 12 is the first aperture spad
	uint8_t spads_enabled = 0;

	for (uint8_t i = 0; i < 48; i++) {
		if (i < first_spad_to_enable || spads_enabled == spad_count) {
			// This bit is lower than the first one that should be enabled, or
			// (reference_spad_count) bits have already been enabled, so zero this bit
			ref_spad_map[i / 8] &= ~(1 << (i % 8));
		} else if ((ref_spad_map[i / 8] >> (i % 8)) & 0x1) {
			spads_enabled++;
		}
	}

	HAL_I2C_Mem_Write(&hi2c2, VLX_CURRENT_ADRESS,
			GLOBAL_CONFIG_SPAD_ENABLES_REF_0, I2C_MEMADD_SIZE_8BIT,
			ref_spad_map, 6, 5);
	// -- VL53L0X_set_reference_spads() end

	// -- VL53L0X_load_tuning_settings() begin
	// DefaultTuningSettings from vl53l0x_tuning.h

	writeReg(VLX_CURRENT_ADRESS, 0xFF, 0x01);
	writeReg(VLX_CURRENT_ADRESS, 0x00, 0x00);

	writeReg(VLX_CURRENT_ADRESS, 0xFF, 0x00);
	writeReg(VLX_CURRENT_ADRESS, 0x09, 0x00);
	writeReg(VLX_CURRENT_ADRESS, 0x10, 0x00);
	writeReg(VLX_CURRENT_ADRESS, 0x11, 0x00);

	writeReg(VLX_CURRENT_ADRESS, 0x24, 0x01);
	writeReg(VLX_CURRENT_ADRESS, 0x25, 0xFF);
	writeReg(VLX_CURRENT_ADRESS, 0x75, 0x00);

	writeReg(VLX_CURRENT_ADRESS, 0xFF, 0x01);
	writeReg(VLX_CURRENT_ADRESS, 0x4E, 0x2C);
	writeReg(VLX_CURRENT_ADRESS, 0x48, 0x00);
	writeReg(VLX_CURRENT_ADRESS, 0x30, 0x20);

	writeReg(VLX_CURRENT_ADRESS, 0xFF, 0x00);
	writeReg(VLX_CURRENT_ADRESS, 0x30, 0x09);
	writeReg(VLX_CURRENT_ADRESS, 0x54, 0x00);
	writeReg(VLX_CURRENT_ADRESS, 0x31, 0x04);
	writeReg(VLX_CURRENT_ADRESS, 0x32, 0x03);
	writeReg(VLX_CURRENT_ADRESS, 0x40, 0x83);
	writeReg(VLX_CURRENT_ADRESS, 0x46, 0x25);
	writeReg(VLX_CURRENT_ADRESS, 0x60, 0x00);
	writeReg(VLX_CURRENT_ADRESS, 0x27, 0x00);
	writeReg(VLX_CURRENT_ADRESS, 0x50, 0x06);
	writeReg(VLX_CURRENT_ADRESS, 0x51, 0x00);
	writeReg(VLX_CURRENT_ADRESS, 0x52, 0x96);
	writeReg(VLX_CURRENT_ADRESS, 0x56, 0x08);
	writeReg(VLX_CURRENT_ADRESS, 0x57, 0x30);
	writeReg(VLX_CURRENT_ADRESS, 0x61, 0x00);
	writeReg(VLX_CURRENT_ADRESS, 0x62, 0x00);
	writeReg(VLX_CURRENT_ADRESS, 0x64, 0x00);
	writeReg(VLX_CURRENT_ADRESS, 0x65, 0x00);
	writeReg(VLX_CURRENT_ADRESS, 0x66, 0xA0);

	writeReg(VLX_CURRENT_ADRESS, 0xFF, 0x01);
	writeReg(VLX_CURRENT_ADRESS, 0x22, 0x32);
	writeReg(VLX_CURRENT_ADRESS, 0x47, 0x14);
	writeReg(VLX_CURRENT_ADRESS, 0x49, 0xFF);
	writeReg(VLX_CURRENT_ADRESS, 0x4A, 0x00);

	writeReg(VLX_CURRENT_ADRESS, 0xFF, 0x00);
	writeReg(VLX_CURRENT_ADRESS, 0x7A, 0x0A);
	writeReg(VLX_CURRENT_ADRESS, 0x7B, 0x00);
	writeReg(VLX_CURRENT_ADRESS, 0x78, 0x21);

	writeReg(VLX_CURRENT_ADRESS, 0xFF, 0x01);
	writeReg(VLX_CURRENT_ADRESS, 0x23, 0x34);
	writeReg(VLX_CURRENT_ADRESS, 0x42, 0x00);
	writeReg(VLX_CURRENT_ADRESS, 0x44, 0xFF);
	writeReg(VLX_CURRENT_ADRESS, 0x45, 0x26);
	writeReg(VLX_CURRENT_ADRESS, 0x46, 0x05);
	writeReg(VLX_CURRENT_ADRESS, 0x40, 0x40);
	writeReg(VLX_CURRENT_ADRESS, 0x0E, 0x06);
	writeReg(VLX_CURRENT_ADRESS, 0x20, 0x1A);
	writeReg(VLX_CURRENT_ADRESS, 0x43, 0x40);

	writeReg(VLX_CURRENT_ADRESS, 0xFF, 0x00);
	writeReg(VLX_CURRENT_ADRESS, 0x34, 0x03);
	writeReg(VLX_CURRENT_ADRESS, 0x35, 0x44);

	writeReg(VLX_CURRENT_ADRESS, 0xFF, 0x01);
	writeReg(VLX_CURRENT_ADRESS, 0x31, 0x04);
	writeReg(VLX_CURRENT_ADRESS, 0x4B, 0x09);
	writeReg(VLX_CURRENT_ADRESS, 0x4C, 0x05);
	writeReg(VLX_CURRENT_ADRESS, 0x4D, 0x04);

	writeReg(VLX_CURRENT_ADRESS, 0xFF, 0x00);
	writeReg(VLX_CURRENT_ADRESS, 0x44, 0x00);
	writeReg(VLX_CURRENT_ADRESS, 0x45, 0x20);
	writeReg(VLX_CURRENT_ADRESS, 0x47, 0x08);
	writeReg(VLX_CURRENT_ADRESS, 0x48, 0x28);
	writeReg(VLX_CURRENT_ADRESS, 0x67, 0x00);
	writeReg(VLX_CURRENT_ADRESS, 0x70, 0x04);
	writeReg(VLX_CURRENT_ADRESS, 0x71, 0x01);
	writeReg(VLX_CURRENT_ADRESS, 0x72, 0xFE);
	writeReg(VLX_CURRENT_ADRESS, 0x76, 0x00);
	writeReg(VLX_CURRENT_ADRESS, 0x77, 0x00);

	writeReg(VLX_CURRENT_ADRESS, 0xFF, 0x01);
	writeReg(VLX_CURRENT_ADRESS, 0x0D, 0x01);

	writeReg(VLX_CURRENT_ADRESS, 0xFF, 0x00);
	writeReg(VLX_CURRENT_ADRESS, 0x80, 0x01);
	writeReg(VLX_CURRENT_ADRESS, 0x01, 0xF8);

	writeReg(VLX_CURRENT_ADRESS, 0xFF, 0x01);
	writeReg(VLX_CURRENT_ADRESS, 0x8E, 0x01);
	writeReg(VLX_CURRENT_ADRESS, 0x00, 0x01);
	writeReg(VLX_CURRENT_ADRESS, 0xFF, 0x00);
	writeReg(VLX_CURRENT_ADRESS, 0x80, 0x00);

	// -- VL53L0X_load_tuning_settings() end

	// "Set interrupt config to new sample ready"
	// -- VL53L0X_SetGpioConfig() begin

	writeReg(VLX_CURRENT_ADRESS, SYSTEM_INTERRUPT_CONFIG_GPIO, 0x04);
	writeReg(VLX_CURRENT_ADRESS, GPIO_HV_MUX_ACTIVE_HIGH,
			readReg(VLX_CURRENT_ADRESS, GPIO_HV_MUX_ACTIVE_HIGH) & ~0x10); // active low
	writeReg(VLX_CURRENT_ADRESS, SYSTEM_INTERRUPT_CLEAR, 0x01);

	// -- VL53L0X_SetGpioConfig() end

	measurement_timing_budget_us = getMeasurementTimingBudget();

	// "Disable MSRC and TCC by default"
	// MSRC = Minimum Signal Rate Check
	// TCC = Target CentreCheck
	// -- VL53L0X_SetSequenceStepEnable() begin

	writeReg(VLX_CURRENT_ADRESS, SYSTEM_SEQUENCE_CONFIG, 0xE8);

	// -- VL53L0X_SetSequenceStepEnable() end

	// "Recalculate timing budget"
	setMeasurementTimingBudget(measurement_timing_budget_us);

	// VL53L0X_StaticInit() end

	// VL53L0X_PerformRefCalibration() begin (VL53L0X_perform_ref_calibration())

	// -- VL53L0X_perform_vhv_calibration() begin

	writeReg(VLX_CURRENT_ADRESS, SYSTEM_SEQUENCE_CONFIG, 0x01);
	if (!performSingleRefCalibration(0x40)) {
		return false;
	}

	// -- VL53L0X_perform_vhv_calibration() end

	// -- VL53L0X_perform_phase_calibration() begin

	writeReg(VLX_CURRENT_ADRESS, SYSTEM_SEQUENCE_CONFIG, 0x02);
	if (!performSingleRefCalibration(0x00)) {
		return false;
	}

	// -- VL53L0X_perform_phase_calibration() end

	// "restore the previous Sequence Config"
	writeReg(VLX_CURRENT_ADRESS, SYSTEM_SEQUENCE_CONFIG, 0xE8);

	// VL53L0X_PerformRefCalibration() end

	return true;
}
bool performSingleRefCalibration(uint8_t vhv_init_byte) {
	writeReg(VLX_CURRENT_ADRESS, SYSRANGE_START, 0x01 | vhv_init_byte); // VL53L0X_REG_SYSRANGE_MODE_START_STOP

	while ((readReg(VLX_CURRENT_ADRESS, RESULT_INTERRUPT_STATUS) & 0x07) == 0) {
	}

	writeReg(VLX_CURRENT_ADRESS, SYSTEM_INTERRUPT_CLEAR, 0x01);

	writeReg(VLX_CURRENT_ADRESS, SYSRANGE_START, 0x00);

	return true;
}
bool setMeasurementTimingBudget(uint32_t budget_us) {
	SequenceStepEnables enables;
	SequenceStepTimeouts timeouts;

	uint16_t const StartOverhead = 1320; // note that this is different than the value in get_
	uint16_t const EndOverhead = 960;
	uint16_t const MsrcOverhead = 660;
	uint16_t const TccOverhead = 590;
	uint16_t const DssOverhead = 690;
	uint16_t const PreRangeOverhead = 660;
	uint16_t const FinalRangeOverhead = 550;

	uint32_t const MinTimingBudget = 20000;

	if (budget_us < MinTimingBudget) {
		return false;
	}

	uint32_t used_budget_us = StartOverhead + EndOverhead;

	getSequenceStepEnables(&enables);
	getSequenceStepTimeouts(&enables, &timeouts);

	if (enables.tcc) {
		used_budget_us += (timeouts.msrc_dss_tcc_us + TccOverhead);
	}

	if (enables.dss) {
		used_budget_us += 2 * (timeouts.msrc_dss_tcc_us + DssOverhead);
	} else if (enables.msrc) {
		used_budget_us += (timeouts.msrc_dss_tcc_us + MsrcOverhead);
	}

	if (enables.pre_range) {
		used_budget_us += (timeouts.pre_range_us + PreRangeOverhead);
	}

	if (enables.final_range) {
		used_budget_us += FinalRangeOverhead;

		// "Note that the final range timeout is determined by the timing
		// budget and the sum of all other timeouts within the sequence.
		// If there is no room for the final range timeout, then an error
		// will be set. Otherwise the remaining time will be applied to
		// the final range."

		if (used_budget_us > budget_us) {
			// "Requested timeout too big."
			return false;
		}

		uint32_t final_range_timeout_us = budget_us - used_budget_us;

		// set_sequence_step_timeout() begin
		// (SequenceStepId == VL53L0X_SEQUENCESTEP_FINAL_RANGE)

		// "For the final range timeout, the pre-range timeout
		//  must be added. To do this both final and pre-range
		//  timeouts must be expressed in macro periods MClks
		//  because they have different vcsel periods."

		uint16_t final_range_timeout_mclks = timeoutMicrosecondsToMclks(
				final_range_timeout_us,
				timeouts.final_range_vcsel_period_pclks);

		if (enables.pre_range) {
			final_range_timeout_mclks += timeouts.pre_range_mclks;
		}

		writeReg16Bit(VLX_CURRENT_ADRESS, FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI,
				encodeTimeout(final_range_timeout_mclks));

		// set_sequence_step_timeout() end

		measurement_timing_budget_us = budget_us; // store for internal reuse
	}
	return true;
}
bool setSignalRateLimit(float limit_Mcps) {
	if (limit_Mcps < 0 || limit_Mcps > 511.99) {
		return false;
	}

	// Q9.7 fixed point format (9 integer bits, 7 fractional bits)
	writeReg16Bit(VLX_CURRENT_ADRESS,
			FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT, limit_Mcps * (1 << 7));
	return true;
}
bool getSpadInfo(uint8_t * count, bool * type_is_aperture) {
	uint8_t tmp;

	writeReg(VLX_CURRENT_ADRESS, 0x80, 0x01);
	writeReg(VLX_CURRENT_ADRESS, 0xFF, 0x01);
	writeReg(VLX_CURRENT_ADRESS, 0x00, 0x00);

	writeReg(VLX_CURRENT_ADRESS, 0xFF, 0x06);
	writeReg(VLX_CURRENT_ADRESS, 0x83,
			readReg(VLX_CURRENT_ADRESS, 0x83) | 0x04);
	writeReg(VLX_CURRENT_ADRESS, 0xFF, 0x07);
	writeReg(VLX_CURRENT_ADRESS, 0x81, 0x01);

	writeReg(VLX_CURRENT_ADRESS, 0x80, 0x01);

	writeReg(VLX_CURRENT_ADRESS, 0x94, 0x6b);
	writeReg(VLX_CURRENT_ADRESS, 0x83, 0x00);
	while (readReg(VLX_CURRENT_ADRESS, 0x83) == 0x00) {
	}
	writeReg(VLX_CURRENT_ADRESS, 0x83, 0x01);
	tmp = readReg(VLX_CURRENT_ADRESS, 0x92);

	*count = tmp & 0x7f;
	*type_is_aperture = (tmp >> 7) & 0x01;

	writeReg(VLX_CURRENT_ADRESS, 0x81, 0x00);
	writeReg(VLX_CURRENT_ADRESS, 0xFF, 0x06);
	writeReg(VLX_CURRENT_ADRESS, 0x83,
			readReg(VLX_CURRENT_ADRESS, 0x83) & ~0x04);
	writeReg(VLX_CURRENT_ADRESS, 0xFF, 0x01);
	writeReg(VLX_CURRENT_ADRESS, 0x00, 0x01);

	writeReg(VLX_CURRENT_ADRESS, 0xFF, 0x00);
	writeReg(VLX_CURRENT_ADRESS, 0x80, 0x00);

	return true;
}
uint32_t getMeasurementTimingBudget(void) {
	SequenceStepEnables enables;
	SequenceStepTimeouts timeouts;

	uint16_t const StartOverhead = 1910; // note that this is different than the value in set_
	uint16_t const EndOverhead = 960;
	uint16_t const MsrcOverhead = 660;
	uint16_t const TccOverhead = 590;
	uint16_t const DssOverhead = 690;
	uint16_t const PreRangeOverhead = 660;
	uint16_t const FinalRangeOverhead = 550;

	// "Start and end overhead times always present"
	uint32_t budget_us = StartOverhead + EndOverhead;

	getSequenceStepEnables(&enables);
	getSequenceStepTimeouts(&enables, &timeouts);

	if (enables.tcc) {
		budget_us += (timeouts.msrc_dss_tcc_us + TccOverhead);
	}

	if (enables.dss) {
		budget_us += 2 * (timeouts.msrc_dss_tcc_us + DssOverhead);
	} else if (enables.msrc) {
		budget_us += (timeouts.msrc_dss_tcc_us + MsrcOverhead);
	}

	if (enables.pre_range) {
		budget_us += (timeouts.pre_range_us + PreRangeOverhead);
	}

	if (enables.final_range) {
		budget_us += (timeouts.final_range_us + FinalRangeOverhead);
	}

	measurement_timing_budget_us = budget_us; // store for internal reuse
	return budget_us;
}
void getSequenceStepEnables(SequenceStepEnables * enables) {
	uint8_t sequence_config = readReg(VLX_CURRENT_ADRESS,
			SYSTEM_SEQUENCE_CONFIG);

	enables->tcc = (sequence_config >> 4) & 0x1;
	enables->dss = (sequence_config >> 3) & 0x1;
	enables->msrc = (sequence_config >> 2) & 0x1;
	enables->pre_range = (sequence_config >> 6) & 0x1;
	enables->final_range = (sequence_config >> 7) & 0x1;
}

// Get sequence step timeouts
// based on get_sequence_step_timeout(),
// but gets all timeouts instead of just the requested one, and also stores
// intermediate values
uint8_t getVcselPulsePeriod(vcselPeriodType type) {
	if (type == VcselPeriodPreRange) {
		return decodeVcselPeriod(
				readReg(VLX_CURRENT_ADRESS, PRE_RANGE_CONFIG_VCSEL_PERIOD));
	} else if (type == VcselPeriodFinalRange) {
		return decodeVcselPeriod(
				readReg(VLX_CURRENT_ADRESS, FINAL_RANGE_CONFIG_VCSEL_PERIOD));
	} else {
		return 255;
	}
}
void getSequenceStepTimeouts(SequenceStepEnables const * enables,
		SequenceStepTimeouts * timeouts) {
	timeouts->pre_range_vcsel_period_pclks = getVcselPulsePeriod(
			VcselPeriodPreRange);

	timeouts->msrc_dss_tcc_mclks = readReg(VLX_CURRENT_ADRESS,
			MSRC_CONFIG_TIMEOUT_MACROP) + 1;
	timeouts->msrc_dss_tcc_us = timeoutMclksToMicroseconds(
			timeouts->msrc_dss_tcc_mclks,
			timeouts->pre_range_vcsel_period_pclks);

	timeouts->pre_range_mclks = decodeTimeout(
			readReg16Bit(VLX_CURRENT_ADRESS,
					PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI));
	timeouts->pre_range_us = timeoutMclksToMicroseconds(
			timeouts->pre_range_mclks, timeouts->pre_range_vcsel_period_pclks);

	timeouts->final_range_vcsel_period_pclks = getVcselPulsePeriod(
			VcselPeriodFinalRange);

	timeouts->final_range_mclks = decodeTimeout(
			readReg16Bit(VLX_CURRENT_ADRESS,
					FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI));

	if (enables->pre_range) {
		timeouts->final_range_mclks -= timeouts->pre_range_mclks;
	}

	timeouts->final_range_us = timeoutMclksToMicroseconds(
			timeouts->final_range_mclks,
			timeouts->final_range_vcsel_period_pclks);
}
uint16_t decodeTimeout(uint16_t reg_val) {
	// format: "(LSByte * 2^MSByte) + 1"
	return (uint16_t) ((reg_val & 0x00FF)
			<< (uint16_t) ((reg_val & 0xFF00) >> 8)) + 1;
}

// Encode sequence step timeout register value from timeout in MCLKs
// based on VL53L0X_encode_timeout()
// Note: the original function took a uint16_t, but the argument passed to it
// is always a uint16_t.
uint16_t encodeTimeout(uint16_t timeout_mclks) {
	// format: "(LSByte * 2^MSByte) + 1"

	uint32_t ls_byte = 0;
	uint16_t ms_byte = 0;

	if (timeout_mclks > 0) {
		ls_byte = timeout_mclks - 1;

		while ((ls_byte & 0xFFFFFF00) > 0) {
			ls_byte >>= 1;
			ms_byte++;
		}

		return (ms_byte << 8) | (ls_byte & 0xFF);
	} else {
		return 0;
	}
}

// Convert sequence step timeout from MCLKs to microseconds with given VCSEL period in PCLKs
// based on VL53L0X_calc_timeout_us()
uint32_t timeoutMclksToMicroseconds(uint16_t timeout_period_mclks,
		uint8_t vcsel_period_pclks) {
	uint32_t macro_period_ns = calcMacroPeriod(vcsel_period_pclks);

	return ((timeout_period_mclks * macro_period_ns) + (macro_period_ns / 2))
			/ 1000;
}

// Convert sequence step timeout from microseconds to MCLKs with given VCSEL period in PCLKs
// based on VL53L0X_calc_timeout_mclks()
uint32_t timeoutMicrosecondsToMclks(uint32_t timeout_period_us,
		uint8_t vcsel_period_pclks) {
	uint32_t macro_period_ns = calcMacroPeriod(vcsel_period_pclks);

	return (((timeout_period_us * 1000) + (macro_period_ns / 2))
			/ macro_period_ns);
}
uint16_t readRangeContinuousMillimeters(void) {

	// assumptions: Linearity Corrective Gain is 1000 (default);
	// fractional ranging is not enabled
	uint16_t range;
	range = readReg16Bit_IT(VLX_CURRENT_ADRESS, RESULT_RANGE_STATUS + 10);

	writeReg_IT(VLX_CURRENT_ADRESS, SYSTEM_INTERRUPT_CLEAR, 0x01);
//	range = readReg16Bit(VLX_CURRENT_ADRESS, RESULT_RANGE_STATUS + 10);
	return range;
}
uint16_t readRangeSemiCont(void) {

	uint16_t range;
	writeReg(VLX_CURRENT_ADRESS, SYSRANGE_START, 0x01);
	while (readReg(VLX_CURRENT_ADRESS, SYSRANGE_START) & 0x01) {
		}
	range = readReg16Bit(VLX_CURRENT_ADRESS, RESULT_RANGE_STATUS + 10);

	writeReg(VLX_CURRENT_ADRESS, SYSTEM_INTERRUPT_CLEAR, 0x01);

	return range;
}
// Performs a single-shot range measurement and returns the reading in
// millimeters
// based on VL53L0X_PerformSingleRangingMeasurement()

uint16_t readRangeSingleMillimeters(void) {

	if(writeReg(VLX_CURRENT_ADRESS, 0x80, 0x01) != HAL_OK)
		return 8192;
	writeReg(VLX_CURRENT_ADRESS, 0xFF, 0x01);
	writeReg(VLX_CURRENT_ADRESS, 0x00, 0x00);
	writeReg(VLX_CURRENT_ADRESS, 0x91, stop_variable);
	writeReg(VLX_CURRENT_ADRESS, 0x00, 0x01);
	writeReg(VLX_CURRENT_ADRESS, 0xFF, 0x00);
	writeReg(VLX_CURRENT_ADRESS, 0x80, 0x00);

	writeReg(VLX_CURRENT_ADRESS, SYSRANGE_START, 0x01);

	// "Wait until start bit has been cleared"
	while (readReg(VLX_CURRENT_ADRESS, SYSRANGE_START) & 0x01) {
	}

	return readRangeContinuousMillimeters();
}
void VL53L0X_startContinuous(uint32_t period_ms)
{
  writeReg(VLX_CURRENT_ADRESS, 0x80, 0x01);
  writeReg(VLX_CURRENT_ADRESS, 0xFF, 0x01);
  writeReg(VLX_CURRENT_ADRESS, 0x00, 0x00);
  writeReg(VLX_CURRENT_ADRESS, 0x91, stop_variable);
  writeReg(VLX_CURRENT_ADRESS, 0x00, 0x01);
  writeReg(VLX_CURRENT_ADRESS, 0xFF, 0x00);
  writeReg(VLX_CURRENT_ADRESS, 0x80, 0x00);

  if (period_ms != 0)
  {
    // continuous timed mode

    // VL53L0X_SetInterMeasurementPeriodMilliSeconds() begin

    uint16_t osc_calibrate_val = readReg16Bit(VLX_CURRENT_ADRESS, OSC_CALIBRATE_VAL);

    if (osc_calibrate_val != 0)
    {
      period_ms *= osc_calibrate_val;
    }

    writeReg32Bit(VLX_CURRENT_ADRESS, SYSTEM_INTERMEASUREMENT_PERIOD, period_ms);

    // VL53L0X_SetInterMeasurementPeriodMilliSeconds() end

    writeReg(VLX_CURRENT_ADRESS, SYSRANGE_START, 0x04); // VL53L0X_REG_SYSRANGE_MODE_TIMED
  }
  else
  {
    // continuous back-to-back mode
    writeReg(VLX_CURRENT_ADRESS, SYSRANGE_START, 0x02); // VL53L0X_REG_SYSRANGE_MODE_BACKTOBACK
  }
}
uint8_t readReg(uint16_t adress, uint8_t reg) {
	uint8_t value;

	HAL_I2C_Mem_Read(&hi2c2, adress, reg, I2C_MEMADD_SIZE_8BIT, i2cRxBuffer, 1, 2);
	value = i2cRxBuffer[0];

	return value;
}

// Read a 16-bit register
uint16_t readReg16Bit(uint16_t adress, uint8_t reg) {
	uint16_t value;

	if (HAL_I2C_Mem_Read(&hi2c2, adress, reg, I2C_MEMADD_SIZE_8BIT, i2cRxBuffer,
			2, 2) == HAL_OK) {
		value = (uint16_t) i2cRxBuffer[0] << 8;
		value |= i2cRxBuffer[1];
		return value;
	} else
		return 8192;
}
uint16_t readReg16Bit_IT(uint16_t adress, uint8_t reg) {
	uint16_t value;
	if (HAL_I2C_Mem_Read_IT(&hi2c2, adress, reg, I2C_MEMADD_SIZE_8BIT,
			i2cRxBuffer, 2) == HAL_OK) {
		osSemaphoreWait(i2c2_semaphore, osWaitForever);
		value = (uint16_t) i2cRxBuffer[0] << 8;
		value |= i2cRxBuffer[1];
		return value;
	} else
		return 8192;
}
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c) {
	if (hi2c->Instance == hi2c2.Instance) {
		osSemaphoreRelease(i2c2_semaphore);
	}
}
// Read a 32-bit register
uint32_t readReg32Bit(uint16_t adress, uint8_t reg) {
	uint32_t value;

	HAL_I2C_Mem_Read(&hi2c2, adress, reg, I2C_MEMADD_SIZE_8BIT, i2cRxBuffer, 4,
			2);
	value = (uint32_t) i2cRxBuffer[0] << 24;
	value |= (uint32_t) i2cRxBuffer[1] << 16;
	value |= (uint16_t) i2cRxBuffer[2] << 8;
	value |= i2cRxBuffer[3];

	return value;
}
int writeReg(uint16_t adress, uint8_t reg, uint8_t value) {
	int status = 1;
	i2cTxBuffer[0] = value;
	status = HAL_I2C_Mem_Write(&hi2c2, adress, reg, I2C_MEMADD_SIZE_8BIT, i2cTxBuffer, 1,
			2);
	return status;
}
int writeReg_IT(uint16_t adress, uint8_t reg, uint8_t value) {
	int status = 1;
	i2cTxBuffer[0] = value;
	status = HAL_I2C_Mem_Write_IT(&hi2c2, adress, reg, I2C_MEMADD_SIZE_8BIT, i2cTxBuffer, 1);
	return status;
}
void writeReg16Bit(uint16_t adress, uint8_t reg, uint16_t value) {
	i2cTxBuffer[0] = (value >> 8) & 0xFF;
	i2cTxBuffer[1] = value & 0xFF;
	HAL_I2C_Mem_Write(&hi2c2, adress, reg, I2C_MEMADD_SIZE_8BIT, i2cTxBuffer, 2,
			2);
}
void writeReg32Bit(uint16_t adress, uint8_t reg, uint32_t value) {
	i2cTxBuffer[0] = (value >> 24) & 0xFF;
	i2cTxBuffer[1] = (value >> 16) & 0xFF;
	i2cTxBuffer[2] = (value >> 8) & 0xFF;
	i2cTxBuffer[3] = value & 0xFF;
	HAL_I2C_Mem_Write(&hi2c2, adress, reg, I2C_MEMADD_SIZE_8BIT, i2cTxBuffer, 4,
			2);
}
