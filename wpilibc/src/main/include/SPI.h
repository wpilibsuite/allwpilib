/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __SPI_H__
#define __SPI_H__

#include "HAL/HAL.h"
#include "SensorBase.h"

class DigitalOutput;
class DigitalInput;

/**
 * SPI bus interface class.
 *
 * This class is intended to be used by sensor (and other SPI device) drivers.
 * It probably should not be used directly.
 *
 * The FPGA only supports a single SPI interface.
 */
class SPI : public SensorBase
{
public:
	SPI(DigitalOutput &clk, DigitalOutput &mosi, DigitalInput &miso);
	SPI(DigitalOutput *clk, DigitalOutput *mosi, DigitalInput *miso);
	SPI(DigitalOutput &clk, DigitalOutput &mosi);
	SPI(DigitalOutput *clk, DigitalOutput *mosi);
	SPI(DigitalOutput &clk, DigitalInput &miso);
	SPI(DigitalOutput *clk, DigitalInput *miso);
	virtual ~SPI();

	void SetBitsPerWord(uint32_t bits);
	uint32_t GetBitsPerWord();
	void SetClockRate(double hz);

	void SetMSBFirst();
	void SetLSBFirst();

	void SetSampleDataOnFalling();
	void SetSampleDataOnRising();

	void SetSlaveSelect(DigitalOutput *ss, tFrameMode mode=kChipSelect, bool activeLow=false);
	void SetSlaveSelect(DigitalOutput &ss, tFrameMode mode=kChipSelect, bool activeLow=false);
	DigitalOutput *GetSlaveSelect(tFrameMode *mode=NULL, bool *activeLow=NULL);

	void SetClockActiveLow();
	void SetClockActiveHigh();

	virtual void ApplyConfig();

	virtual uint16_t GetOutputFIFOAvailable();
	virtual uint16_t GetNumReceived();

	virtual bool IsDone();
	bool HadReceiveOverflow();

	virtual void Write(uint32_t data);
	virtual uint32_t Read(bool initiate = false);

	virtual void Reset();
	virtual void ClearReceivedData();

protected:
	void* m_spi;
	DigitalOutput *m_ss;

private:
	void Init(DigitalOutput *clk, DigitalOutput *mosi, DigitalInput *miso);

	DISALLOW_COPY_AND_ASSIGN(SPI);
};

#endif
