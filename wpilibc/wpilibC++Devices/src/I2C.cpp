/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "I2C.h"
#include "HAL/HAL.hpp"
#include "HAL/Digital.hpp"
#include "WPIErrors.h"

/**
 * Constructor.
 *
 * @param port The I2C port to which the device is connected.
 * @param deviceAddress The address of the device on the I2C bus.
 */
I2C::I2C(Port port, uint8_t deviceAddress) :
	m_port (port)
	, m_deviceAddress (deviceAddress)
{
	int32_t status = 0;
	i2CInitialize(m_port, &status);
	//wpi_setErrorWithContext(status, getHALErrorMessage(status));

	HALReport(HALUsageReporting::kResourceType_I2C, deviceAddress);
}

/**
 * Destructor.
 */
I2C::~I2C()
{
	i2CClose(m_port);
}

/**
 * Generic transaction.
 *
 * This is a lower-level interface to the I2C hardware giving you more control over each transaction.
 *
 * @param dataToSend Buffer of data to send as part of the transaction.
 * @param sendSize Number of bytes to send as part of the transaction. [0..6]
 * @param dataReceived Buffer to read data into.
 * @param receiveSize Number of bytes to read from the device. [0..7]
 * @return Transfer Aborted... false for success, true for aborted.
 */
bool I2C::Transaction(uint8_t *dataToSend, uint8_t sendSize, uint8_t *dataReceived, uint8_t receiveSize)
{
	if (sendSize > 6) // Optional, provides better error message.
	{
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "sendSize");
		return true;
	}
	if (receiveSize > 7) // Optional, provides better error message.
	{
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "receiveSize");
		return true;
	}

	int32_t status = 0;
	status = i2CTransaction(m_port, m_deviceAddress,
											dataToSend, sendSize, dataReceived, receiveSize);
	//wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return status < 0;
}

/**
 * Attempt to address a device on the I2C bus.
 *
 * This allows you to figure out if there is a device on the I2C bus that
 * responds to the address specified in the constructor.
 *
 * @return Transfer Aborted... false for success, true for aborted.
 */
bool I2C::AddressOnly()
{
	int32_t status = 0;
	status = Transaction(NULL, 0, NULL, 0);
	return status < 0;
}

/**
 * Execute a write transaction with the device.
 *
 * Write a single byte to a register on a device and wait until the
 *   transaction is complete.
 *
 * @param registerAddress The address of the register on the device to be written.
 * @param data The byte to write to the register on the device.
 * @return Transfer Aborted... false for success, true for aborted.
 */
bool I2C::Write(uint8_t registerAddress, uint8_t data)
{
	uint8_t buffer[2];
	buffer[0] = registerAddress;
	buffer[1] = data;
	int32_t status = 0;
	status = i2CWrite(m_port, m_deviceAddress, buffer, sizeof(buffer));
	return status < 0;
}

/**
 * Execute a bulk write transaction with the device.
 *
 * Write multiple bytes to a device and wait until the
 *   transaction is complete.
 *
 * @param data The data to write to the register on the device.
 * @param count The number of bytes to be written.
 * @return Transfer Aborted... false for success, true for aborted.
 */
bool I2C::WriteBulk(uint8_t* data, uint8_t count)
{
	int32_t status = 0;
	status = i2CWrite(m_port, m_deviceAddress, data, count);
	return status < 0;
}

/**
 * Execute a read transaction with the device.
 *
 * Read 1 to 7 bytes from a device.
 * Most I2C devices will auto-increment the register pointer internally
 *   allowing you to read up to 7 consecutive registers on a device in a
 *   single transaction.
 *
 * @param registerAddress The register to read first in the transaction.
 * @param count The number of bytes to read in the transaction. [1..7]
 * @param buffer A pointer to the array of bytes to store the data read from the device.
 * @return Transfer Aborted... false for success, true for aborted.
 */
bool I2C::Read(uint8_t registerAddress, uint8_t count, uint8_t *buffer)
{
	if (count < 1 || count > 7)
	{
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "count");
		return true;
	}
	if (buffer == NULL)
	{
		wpi_setWPIErrorWithContext(NullParameter, "buffer");
		return true;
	}
	int32_t status = 0;
	status = Transaction(&registerAddress, sizeof(registerAddress), buffer, count);
	return status < 0;
}

/**
 * Execute a read only transaction with the device.
 *
 * Read 1 to 7 bytes from a device. This method does not write any data to prompt
 * the device.
 *
 * @param buffer
 *            A pointer to the array of bytes to store the data read from
 *            the device.
 * @param count
 *            The number of bytes to read in the transaction. [1..7]
 * @return Transfer Aborted... false for success, true for aborted.
 */
bool I2C::ReadOnly(uint8_t count, uint8_t *buffer)
{
	if (count < 1 || count > 7)
	{
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "count");
		return true;
	}
	if (buffer == NULL)
	{
		wpi_setWPIErrorWithContext(NullParameter, "buffer");
		return true;
	}
	int32_t status = 0;
	status = i2CRead(m_port, m_deviceAddress, buffer, count);
	return status < 0;
}

/**
 * Send a broadcast write to all devices on the I2C bus.
 *
 * This is not currently implemented!
 *
 * @param registerAddress The register to write on all devices on the bus.
 * @param data The value to write to the devices.
 */
void I2C::Broadcast(uint8_t registerAddress, uint8_t data)
{
}

/**
 * Verify that a device's registers contain expected values.
 *
 * Most devices will have a set of registers that contain a known value that
 *   can be used to identify them.  This allows an I2C device driver to easily
 *   verify that the device contains the expected value.
 *
 * @pre The device must support and be configured to use register auto-increment.
 *
 * @param registerAddress The base register to start reading from the device.
 * @param count The size of the field to be verified.
 * @param expected A buffer containing the values expected from the device.
 */
bool I2C::VerifySensor(uint8_t registerAddress, uint8_t count, const uint8_t *expected)
{
	// TODO: Make use of all 7 read bytes
	uint8_t deviceData[4];
	for (uint8_t i=0, curRegisterAddress = registerAddress; i < count; i+=4, curRegisterAddress+=4)
	{
		uint8_t toRead = count - i < 4 ? count - i : 4;
		// Read the chunk of data.  Return false if the sensor does not respond.
		if (Read(curRegisterAddress, toRead, deviceData)) return false;

		for (uint8_t j=0; j<toRead; j++)
		{
			if(deviceData[j] != expected[i + j]) return false;
		}
	}
	return true;
}
