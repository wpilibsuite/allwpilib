/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "SPI.h"

#include "DigitalModule.h"
#include "DigitalInput.h"
#include "DigitalOutput.h"
#include "NetworkCommunication/UsageReporting.h"
#include "HAL/cpp/Synchronized.h"
#include "WPIErrors.h"

#include <math.h>

/**
 * Constructor for input and output.
 *
 * @param clk	The digital output for the clock signal.
 * @param mosi	The digital output for the written data to the slave
 *				(master-out slave-in).
 * @param miso	The digital input for the input data from the slave
 *				(master-in slave-out).
 */
SPI::SPI(DigitalOutput &clk, DigitalOutput &mosi, DigitalInput &miso)
{
	Init(&clk, &mosi, &miso);
}

/**
 * Constructor for input and output.
 *
 * @param clk	The digital output for the clock signal.
 * @param mosi	The digital output for the written data to the slave
 *				(master-out slave-in).
 * @param miso	The digital input for the input data from the slave
 *				(master-in slave-out).
 */
SPI::SPI(DigitalOutput *clk, DigitalOutput *mosi, DigitalInput *miso)
{
	Init(clk, mosi, miso);
}

/**
 * Constructor for output only.
 *
 * @param clk	The digital output for the clock signal.
 * @param mosi	The digital output for the written data to the slave
 *				(master-out slave-in).
 */
SPI::SPI(DigitalOutput &clk, DigitalOutput &mosi)
{
	Init(&clk, &mosi, NULL);
}

/**
 * Constructor for output only.
 *
 * @param clk	The digital output for the clock signal.
 * @param mosi	The digital output for the written data to the slave
 *				(master-out slave-in).
 */
SPI::SPI(DigitalOutput *clk, DigitalOutput *mosi)
{
	Init(clk, mosi, NULL);
}

/**
 * Constructor for input only.
 *
 * @param clk	The digital output for the clock signal.
 * @param miso	The digital input for the input data from the slave
 *				(master-in slave-out).
 */
SPI::SPI(DigitalOutput &clk, DigitalInput &miso)
{
	Init(&clk, NULL, &miso);
}

/**
 * Constructor for input only.
 *
 * @param clk	The digital output for the clock signal.
 * @param miso	The digital input for the input data from the slave
 *				(master-in slave-out).
 */
SPI::SPI(DigitalOutput *clk, DigitalInput *miso)
{
	Init(clk, NULL, miso);
}

/**
 * Destructor.
 */
SPI::~SPI()
{
	int32_t status = 0;
	cleanSPI(m_spi, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Initialize SPI channel configuration.
 *
 * @param clk	The digital output for the clock signal.
 * @param mosi	The digital output for the written data to the slave
 *				(master-out slave-in).
 * @param miso	The digital input for the input data from the slave
 *				(master-in slave-out).
 */
void SPI::Init(DigitalOutput *clk, DigitalOutput *mosi, DigitalInput *miso)
{
  	int32_t status = 0;
	m_spi = initializeSPI(clk->GetModuleForRouting(), clk->GetChannelForRouting(),
						  mosi->GetModuleForRouting(), mosi->GetChannelForRouting(),
						  miso->GetModuleForRouting(), miso->GetChannelForRouting(), &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));

	m_ss = NULL;

	static int32_t instances = 0;
	instances++;
	nUsageReporting::report(nUsageReporting::kResourceType_SPI, instances);
}

/**
 * Configure the number of bits from each word that the slave transmits
 * or receives.
 *
 * @param bits	The number of bits in one frame (1 to 32 bits).
 */
void SPI::SetBitsPerWord(uint32_t bits)
{
	int32_t status = 0;
	setSPIBitsPerWord(m_spi, bits, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));	
}

/**
 * Get the number of bits from each word that the slave transmits
 * or receives.
 *
 * @return The number of bits in one frame (1 to 32 bits).
 */
uint32_t SPI::GetBitsPerWord()
{
	int32_t status = 0;
	uint32_t bits = getSPIBitsPerWord(m_spi, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));	
	return bits;
}

/**
 * Configure the rate of the generated clock signal.
 * The default and maximum value is 76,628.4 Hz.
 *
 * @param hz	The clock rate in Hertz.
 */
void SPI::SetClockRate(double hz)
{
	int32_t status = 0;
	setSPIClockRate(m_spi, hz, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Configure the order that bits are sent and received on the wire
 * to be most significant bit first.
 */
void SPI::SetMSBFirst()
{
	int32_t status = 0;
	setSPIMSBFirst(m_spi, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Configure the order that bits are sent and received on the wire
 * to be least significant bit first.
 */
void SPI::SetLSBFirst()
{
	int32_t status = 0;
	setSPILSBFirst(m_spi, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Configure that the data is stable on the falling edge and the data
 * changes on the rising edge.
 */
void SPI::SetSampleDataOnFalling()
{
	int32_t status = 0;
	setSPISampleDataOnFalling(m_spi, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Configure that the data is stable on the rising edge and the data
 * changes on the falling edge.
 */
void SPI::SetSampleDataOnRising()
{
	int32_t status = 0;
	setSPISampleDataOnRising(m_spi, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Configure the slave select line behavior.
 *
 * @param ss slave select digital output.
 * @param mode Frame mode:
 *			   kChipSelect: active for the duration of the frame.
 *			   kPreLatchPulse: pulses before the transfer of each frame.
 *			   kPostLatchPulse: pulses after the transfer of each frame.
 *			   kPreAndPostLatchPulse: pulses before and after each frame.
 * @param activeLow True if slave select line is active low.
 */
void SPI::SetSlaveSelect(DigitalOutput *ss, tFrameMode mode, bool activeLow)
{
	int32_t status = 0;

	if (ss)
	{
		setSPISlaveSelect(m_spi, ss->GetModuleForRouting(), ss->GetChannelForRouting(), &status);
	}
	else
	{
		setSPISlaveSelect(m_spi, 0, 0, &status);
	}
	m_ss = ss;

	setSPILatchMode(m_spi, mode, &status);
	setSPIFramePolarity(m_spi, activeLow, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Configure the slave select line behavior.
 *
 * @param ss slave select digital output.
 * @param mode Frame mode:
 *			   kChipSelect: active for the duration of the frame.
 *			   kPreLatchPulse: pulses before the transfer of each frame.
 *			   kPostLatchPulse: pulses after the transfer of each frame.
 *			   kPreAndPostLatchPulse: pulses before and after each frame.
 * @param activeLow True if slave select line is active low.
 */
void SPI::SetSlaveSelect(DigitalOutput &ss, tFrameMode mode, bool activeLow)
{
	SetSlaveSelect(&ss, mode, activeLow);
}

/**
 * Get the slave select line behavior.
 *
 * @param mode Frame mode:
 *			   kChipSelect: active for the duration of the frame.
 *			   kPreLatchPulse: pulses before the transfer of each frame.
 *			   kPostLatchPulse: pulses after the transfer of each frame.
 *			   kPreAndPostLatchPulse: pulses before and after each frame.
 * @param activeLow True if slave select line is active low.
 * @return The slave select digital output.
 */
DigitalOutput *SPI::GetSlaveSelect(tFrameMode *mode, bool *activeLow)
{
	int32_t status = 0;
	if (mode != NULL)
	{
		*mode = getSPILatchMode(m_spi, &status);
	}
	if (activeLow != NULL)
	{
		*activeLow = getSPIFramePolarity(m_spi, &status);
	}
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return m_ss;
}

/**
 * Configure the clock output line to be active low.
 * This is sometimes called clock polarity high.
 */
void SPI::SetClockActiveLow()
{
	int32_t status = 0;
	setSPIClockActiveLow(m_spi, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Configure the clock output line to be active high.
 * This is sometimes called clock polarity low.
 */
void SPI::SetClockActiveHigh()
{
	int32_t status = 0;
	setSPIClockActiveHigh(m_spi, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Apply configuration settings and reset the SPI logic.
 */
void SPI::ApplyConfig()
{
	int32_t status = 0;
	applySPIConfig(m_spi, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Get the number of words that can currently be stored before being
 * transmitted to the device.
 *
 * @return The number of words available to be written.
 */
uint16_t SPI::GetOutputFIFOAvailable()
{
	int32_t status = 0;
	uint16_t result = getSPIOutputFIFOAvailable(m_spi, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return result;
}

/**
 * Get the number of words received and currently available to be read from
 * the receive FIFO.
 *
 * @return The number of words available to read.
 */
uint16_t SPI::GetNumReceived()
{
	int32_t status = 0;
	uint16_t result = getSPINumReceived(m_spi, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return result;
}

/**
 * Have all pending transfers completed?
 *
 * @return True if no transfers are pending.
 */
bool SPI::IsDone()
{
	int32_t status = 0;
	bool result = isSPIDone(m_spi, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return result;
}

/**
 * Determine if the receive FIFO was full when attempting to add new data at
 * end of a transfer.
 *
 * @return True if the receive FIFO overflowed.
 */
bool SPI::HadReceiveOverflow()
{
	int32_t status = 0;
	bool result = hadSPIReceiveOverflow(m_spi, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return result;
}

/**
 * Write a word to the slave device.  Blocks until there is space in the
 * output FIFO.
 *
 * If not running in output only mode, also saves the data received
 * on the MISO input during the transfer into the receive FIFO.
 */
void SPI::Write(uint32_t data)
{
	int32_t status = 0;
	writeSPI(m_spi, data, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Read a word from the receive FIFO.
 *
 * Waits for the current transfer to complete if the receive FIFO is empty.
 *
 * If the receive FIFO is empty, there is no active transfer, and initiate
 * is false, errors.
 *
 * @param initiate	If true, this function pushes "0" into the
 *				    transmit buffer and initiates a transfer.
 *				    If false, this function assumes that data is
 *				    already in the receive FIFO from a previous write.
 */
uint32_t SPI::Read(bool initiate)
{
	int32_t status = 0;
	uint32_t value = readSPI(m_spi, initiate, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return value;
}

/**
 * Stop any transfer in progress and empty the transmit FIFO.
 */
void SPI::Reset()
{
	int32_t status = 0;
	resetSPI(m_spi, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Empty the receive FIFO.
 */
void SPI::ClearReceivedData()
{
	int32_t status = 0;
	clearSPIReceivedData(m_spi, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}
