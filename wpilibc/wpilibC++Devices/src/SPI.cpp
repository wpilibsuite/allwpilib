/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "SPI.h"

#include "WPIErrors.h"
#include "HAL/Digital.hpp"

#include <string.h>


/**
 * Constructor
 *
 * @param SPIport the physical SPI port
 */
SPI::SPI(Port SPIport)
{
	m_port = SPIport;
  	int32_t status = 0;
  	spiInitialize(m_port, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));

	static int32_t instances = 0;
	instances++;
	HALReport(HALUsageReporting::kResourceType_SPI, instances);
}

/**
 * Destructor.
 */
SPI::~SPI()
{
	spiClose(m_port);
}

/**
 * Configure the rate of the generated clock signal.
 * 
 * The default value is 500,000Hz.
 * The maximum value is 4,000,000Hz.
 *
 * @param hz	The clock rate in Hertz.
 */
void SPI::SetClockRate(double hz)
{
	spiSetSpeed(m_port, hz);
}

/**
 * Configure the order that bits are sent and received on the wire
 * to be most significant bit first.
 */
void SPI::SetMSBFirst()
{
	m_msbFirst = true;
	spiSetOpts(m_port, (int) m_msbFirst, (int) m_sampleOnTrailing, (int) m_clk_idle_high);
}

/**
 * Configure the order that bits are sent and received on the wire
 * to be least significant bit first.
 */
void SPI::SetLSBFirst()
{
	m_msbFirst = false;
	spiSetOpts(m_port, (int) m_msbFirst, (int) m_sampleOnTrailing, (int) m_clk_idle_high);
}

/**
 * Configure that the data is stable on the falling edge and the data
 * changes on the rising edge.
 */
void SPI::SetSampleDataOnFalling()
{
	m_sampleOnTrailing = true;
	spiSetOpts(m_port, (int) m_msbFirst, (int) m_sampleOnTrailing, (int) m_clk_idle_high);
}

/**
 * Configure that the data is stable on the rising edge and the data
 * changes on the falling edge.
 */
void SPI::SetSampleDataOnRising()
{
	m_sampleOnTrailing = false;
	spiSetOpts(m_port, (int) m_msbFirst, (int) m_sampleOnTrailing, (int) m_clk_idle_high);
}

/**
 * Configure the clock output line to be active low.
 * This is sometimes called clock polarity high or clock idle high.
 */
void SPI::SetClockActiveLow()
{
	m_clk_idle_high = true;
	spiSetOpts(m_port, (int) m_msbFirst, (int) m_sampleOnTrailing, (int) m_clk_idle_high);
}

/**
 * Configure the clock output line to be active high.
 * This is sometimes called clock polarity low or clock idle low.
 */
void SPI::SetClockActiveHigh()
{
	m_clk_idle_high = false;
	spiSetOpts(m_port, (int) m_msbFirst, (int) m_sampleOnTrailing, (int) m_clk_idle_high);
}

/**
 * Configure the chip select line to be active high.
 */
void SPI::SetChipSelectActiveHigh()
{
	int32_t status = 0;
	spiSetChipSelectActiveHigh(m_port, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Configure the chip select line to be active low.
 */
void SPI::SetChipSelectActiveLow()
{
	int32_t status = 0;
	spiSetChipSelectActiveLow(m_port, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}


/**
 * Write data to the slave device.  Blocks until there is space in the
 * output FIFO.
 *
 * If not running in output only mode, also saves the data received
 * on the MISO input during the transfer into the receive FIFO.
 */
int32_t SPI::Write(uint8_t* data, uint8_t size)
{
	int32_t retVal = 0;
	retVal = spiWrite(m_port, data, size);
	return retVal;
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
int32_t SPI::Read(bool initiate, uint8_t* dataReceived, uint8_t size)
{
	int32_t retVal = 0;
	if(initiate){
		uint8_t* dataToSend = new uint8_t[size];
		memset(dataToSend, 0, size);
		retVal = spiTransaction(m_port, dataToSend, dataReceived, size);
	}
	else
		retVal = spiRead(m_port, dataReceived, size);
	return retVal;
}

/**
 * Perform a simultaneous read/write transaction with the device
 *
 * @param dataToSend The data to be written out to the device
 * @param dataReceived Buffer to receive data from the device
 * @param size The length of the transaction, in bytes
 */
int32_t SPI::Transaction(uint8_t* dataToSend, uint8_t* dataReceived, uint8_t size){
	int32_t retVal = 0;
	retVal = spiTransaction(m_port, dataToSend, dataReceived, size);
	return retVal;
}
