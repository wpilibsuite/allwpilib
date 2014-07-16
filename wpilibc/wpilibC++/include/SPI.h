/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "HAL/HAL.hpp"
#include "SensorBase.h"

class DigitalOutput;
class DigitalInput;

/**
 * SPI bus interface class.
 *
 * This class is intended to be used by sensor (and other SPI device) drivers.
 * It probably should not be used directly.
 *
 */
class SPI : public SensorBase
{
public:
	enum Port {kOnboardCS0, kOnboardCS1, kOnboardCS2, kOnboardCS3, kMXP};
	SPI(Port SPIport);
	virtual ~SPI();

	void SetClockRate(double hz);

	void SetMSBFirst();
	void SetLSBFirst();

	void SetSampleDataOnFalling();
	void SetSampleDataOnRising();


	void SetClockActiveLow();
	void SetClockActiveHigh();

	void SetChipSelectActiveHigh();
	void SetChipSelectActiveLow();

	virtual int32_t Write(uint8_t* data, uint8_t size);
	virtual int32_t Read(bool initiate, uint8_t* dataReceived, uint8_t size);
	virtual int32_t Transaction(uint8_t* dataToSend, uint8_t* dataReceived, uint8_t size);


protected:
	uint8_t m_port;
	bool m_msbFirst;
	bool m_sampleOnTrailing;
	bool m_clk_idle_high;

private:
	void Init();

	DISALLOW_COPY_AND_ASSIGN(SPI);
};
