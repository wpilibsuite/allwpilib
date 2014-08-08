/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "interfaces/Accelerometer.h"
#include "SensorBase.h"
#include "SPI.h"

class DigitalInput;
class DigitalOutput;

/**
 * ADXL345 Accelerometer on SPI.
 *
 * This class allows access to an Analog Devices ADXL345 3-axis accelerometer via SPI.
 * This class assumes the sensor is wired in 4-wire SPI mode.
 */
class ADXL345_SPI : public Accelerometer, public SensorBase
{
protected:
	static const uint8_t kPowerCtlRegister = 0x2D;
	static const uint8_t kDataFormatRegister = 0x31;
	static const uint8_t kDataRegister = 0x32;
	static constexpr double kGsPerLSB = 0.00390625;
	enum SPIAddressFields {kAddress_Read=0x80, kAddress_MultiByte=0x40};
	enum PowerCtlFields {kPowerCtl_Link=0x20, kPowerCtl_AutoSleep=0x10, kPowerCtl_Measure=0x08, kPowerCtl_Sleep=0x04};
	enum DataFormatFields {kDataFormat_SelfTest=0x80, kDataFormat_SPI=0x40, kDataFormat_IntInvert=0x20,
		kDataFormat_FullRes=0x08, kDataFormat_Justify=0x04};

public:
	enum Axes {kAxis_X=0x00, kAxis_Y=0x02, kAxis_Z=0x04};
	struct AllAxes
	{
		double XAxis;
		double YAxis;
		double ZAxis;
	};

public:
	ADXL345_SPI(SPI::Port port, Range range=kRange_2G);
	virtual ~ADXL345_SPI();

	// Accelerometer interface
	virtual void SetRange(Range range);
	virtual double GetX();
	virtual double GetY();
	virtual double GetZ();

	virtual double GetAcceleration(Axes axis);
	virtual AllAxes GetAccelerations();

protected:
	void Init(Range range);

	SPI* m_spi;
	SPI::Port m_port;
};
