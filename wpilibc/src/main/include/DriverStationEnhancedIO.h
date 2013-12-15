/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __DRIVER_STATION_ENHANCED_IO_H__
#define __DRIVER_STATION_ENHANCED_IO_H__

#include "ErrorBase.h"
#include "NetworkCommunication/FRCComm.h"
#include <stack>
#include <vector>
#include "HAL/HAL.h"

#include "HAL/Semaphore.h"

#define kAnalogInputResolution ((double)((1<<14)-1))
#define kAnalogInputReference 3.3
#define kAnalogOutputResolution ((double)((1<<8)-1))
#define kAnalogOutputReference 4.0
#define kAccelOffset 8300
#define kAccelScale 3300.0
#define kSupportedAPIVersion 1

/**
 * Interact with the more complete I/O available from the 
 * newest driver station.  Get a reference to an object
 * of this type by calling GetEnhancedIO() on the DriverStation object.
 */
class DriverStationEnhancedIO : public ErrorBase
{
	// Can only be constructed by the DriverStation class.
	friend class DriverStation;

#pragma pack(push,1)
	// BEGIN: Definitions from the Cypress firmware
	typedef struct
	{
		uint16_t digital;
		uint16_t digital_oe;
		uint16_t digital_pe;
		uint16_t pwm_compare[4];
		uint16_t pwm_period[2];
		uint8_t dac[2];
		uint8_t leds;
		union
		{
			struct
			{
				// Bits are inverted from cypress fw because of big-endian!
				uint8_t pwm_enable : 4;
				uint8_t comparator_enable : 2;
				uint8_t quad_index_enable : 2;
			};
			uint8_t enables;
		};
		uint8_t fixed_digital_out;
	} output_t;  //data to IO (23 bytes)

	typedef struct
	{
		uint8_t api_version;
		uint8_t fw_version;
		int16_t analog[8];
		uint16_t digital;
		int16_t accel[3];
		int16_t quad[2];
		uint8_t buttons;
		uint8_t capsense_slider;
		uint8_t capsense_proximity;
	} input_t;	//data from IO (33 bytes)
	// END: Definitions from the Cypress firmware

	// Dynamic block definitions
	typedef struct
	{
		uint8_t size; // Must be 25 (size remaining in the block not counting the size variable)
		uint8_t id; // Must be 18
		output_t data;
		uint8_t flags;
	} status_block_t;

	typedef struct
	{
		uint8_t size; // Must be 34
		uint8_t id; // Must be 17
		input_t data;
	} control_block_t;
#pragma pack(pop)

	enum tBlockID
	{
		kInputBlockID = kFRC_NetworkCommunication_DynamicType_DSEnhancedIO_Input,
		kOutputBlockID = kFRC_NetworkCommunication_DynamicType_DSEnhancedIO_Output,
	};
	enum tStatusFlags {kStatusValid = 0x01, kStatusConfigChanged = 0x02, kForceEnhancedMode = 0x04};

public:
	enum tDigitalConfig {kUnknown, kInputFloating, kInputPullUp, kInputPullDown, kOutput, kPWM, kAnalogComparator};
	enum tAccelChannel {kAccelX = 0, kAccelY = 1, kAccelZ = 2};
	enum tPWMPeriodChannels {kPWMChannels1and2, kPWMChannels3and4};

	double GetAcceleration(tAccelChannel channel);
	double GetAnalogIn(uint32_t channel);
	double GetAnalogInRatio(uint32_t channel);
	double GetAnalogOut(uint32_t channel);
	void SetAnalogOut(uint32_t channel, double value);
	bool GetButton(uint32_t channel);
	uint8_t GetButtons();
	void SetLED(uint32_t channel, bool value);
	void SetLEDs(uint8_t value);
	bool GetDigital(uint32_t channel);
	uint16_t GetDigitals();
	void SetDigitalOutput(uint32_t channel, bool value);
	tDigitalConfig GetDigitalConfig(uint32_t channel);
	void SetDigitalConfig(uint32_t channel, tDigitalConfig config);
	double GetPWMPeriod(tPWMPeriodChannels channels);
	void SetPWMPeriod(tPWMPeriodChannels channels, double period);
	bool GetFixedDigitalOutput(uint32_t channel);
	void SetFixedDigitalOutput(uint32_t channel, bool value);
	int16_t GetEncoder(uint32_t encoderNumber);
	void ResetEncoder(uint32_t encoderNumber);
	bool GetEncoderIndexEnable(uint32_t encoderNumber);
	void SetEncoderIndexEnable(uint32_t encoderNumber, bool enable);
	double GetTouchSlider();
	double GetPWMOutput(uint32_t channel);
	void SetPWMOutput(uint32_t channel, double value);
	uint8_t GetFirmwareVersion();

private:
	DriverStationEnhancedIO();
	virtual ~DriverStationEnhancedIO();
	void UpdateData();
	void MergeConfigIntoOutput(const status_block_t &dsOutputBlock, status_block_t &localCache);
	bool IsConfigEqual(const status_block_t &dsOutputBlock, const status_block_t &localCache);

	// Usage Guidelines...
	DISALLOW_COPY_AND_ASSIGN(DriverStationEnhancedIO);

	control_block_t m_inputData;
	status_block_t m_outputData;
	MUTEX_ID m_inputDataSemaphore;
	MUTEX_ID m_outputDataSemaphore;
	bool m_inputValid;
	bool m_outputValid;
	bool m_configChanged;
	bool m_requestEnhancedEnable;
	int16_t m_encoderOffsets[2];
};

#endif

