/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __ADXL345_SPI_h__
#define __ADXL345_SPI_h__

#include "SensorBase.h"

class DigitalInput;
class DigitalOutput;
class SPI;

/**
 * ADXL345 Accelerometer on SPI.
 * 
 * This class alows access to an Analog Devices ADXL345 3-axis accelerometer via SPI.
 * This class assumes the sensor is wired in 4-wire SPI mode.
 */
class ADXL345_SPI : public SensorBase
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
	enum DataFormat_Range {kRange_2G=0x00, kRange_4G=0x01, kRange_8G=0x02, kRange_16G=0x03};
	enum Axes {kAxis_X=0x00, kAxis_Y=0x02, kAxis_Z=0x04};
	struct AllAxes
	{
		double XAxis;
		double YAxis;
		double ZAxis;
	};

public:
	ADXL345_SPI(DigitalOutput &clk, DigitalOutput &mosi, DigitalInput &miso,
		DigitalOutput &cs, DataFormat_Range range=kRange_2G);
	ADXL345_SPI(DigitalOutput *clk, DigitalOutput *mosi, DigitalInput *miso,
		DigitalOutput *cs, DataFormat_Range range=kRange_2G);
	ADXL345_SPI(uint8_t moduleNumber, uint32_t clk, uint32_t mosi, uint32_t miso, uint32_t cs,
		DataFormat_Range range=kRange_2G);
	virtual ~ADXL345_SPI();
	virtual double GetAcceleration(Axes axis);
	virtual AllAxes GetAccelerations();

protected:
	void Init(DigitalOutput *clk, DigitalOutput *mosi, DigitalInput *miso,
		DigitalOutput *cs, DataFormat_Range range);

	DigitalOutput *m_clk;
	DigitalOutput *m_mosi;
	DigitalInput *m_miso;
	DigitalOutput *m_cs;
	SPI* m_spi;
};

#endif

