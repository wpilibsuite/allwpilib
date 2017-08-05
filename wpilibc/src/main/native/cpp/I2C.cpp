/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/I2C.h"
#include "I2C.h"

#include "HAL/HAL.h"
#include "WPIErrors.h"

using namespace frc;

/**
 * Constructor.
 *
 * @param port          The I2C port to which the device is connected.
 * @param deviceAddress The address of the device on the I2C bus.
 */
I2C::I2C(Port port, int deviceAddress)
    : m_port(static_cast<HAL_I2CPort>(port)), m_deviceAddress(deviceAddress) {
  int32_t status = 0;
  HAL_InitializeI2C(m_port, &status);
  // wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));

  HAL_Report(HALUsageReporting::kResourceType_I2C, deviceAddress);
}

/**
 * Destructor.
 */
I2C::~I2C() { HAL_CloseI2C(m_port); }

/**
 * Generic transaction.
 *
 * This is a lower-level interface to the I2C hardware giving you more control
 * over each transaction.
 *
 * @param dataToSend   Buffer of data to send as part of the transaction.
 * @param sendSize     Number of bytes to send as part of the transaction.
 * @param dataReceived Buffer to read data into.
 * @param receiveSize  Number of bytes to read from the device.
 * @return Transfer Aborted... false for success, true for aborted.
 */
bool I2C::Transaction(uint8_t* dataToSend, int sendSize, uint8_t* dataReceived,
                      int receiveSize) {
  int32_t status = 0;
  status = HAL_TransactionI2C(m_port, m_deviceAddress, dataToSend, sendSize,
                              dataReceived, receiveSize);
  // wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
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
bool I2C::AddressOnly() { return Transaction(nullptr, 0, nullptr, 0); }

/**
 * Execute a write transaction with the device.
 *
 * Write a single byte to a register on a device and wait until the
 *   transaction is complete.
 *
 * @param registerAddress The address of the register on the device to be
 *                        written.
 * @param data            The byte to write to the register on the device.
 * @return Transfer Aborted... false for success, true for aborted.
 */
bool I2C::Write(int registerAddress, uint8_t data) {
  uint8_t buffer[2];
  buffer[0] = registerAddress;
  buffer[1] = data;
  int32_t status = 0;
  status = HAL_WriteI2C(m_port, m_deviceAddress, buffer, sizeof(buffer));
  return status < 0;
}

/**
 * Execute a bulk write transaction with the device.
 *
 * Write multiple bytes to a device and wait until the
 *   transaction is complete.
 *
 * @param data  The data to write to the register on the device.
 * @param count The number of bytes to be written.
 * @return Transfer Aborted... false for success, true for aborted.
 */
bool I2C::WriteBulk(uint8_t* data, int count) {
  int32_t status = 0;
  status = HAL_WriteI2C(m_port, m_deviceAddress, data, count);
  return status < 0;
}

/**
 * Execute a read transaction with the device.
 *
 * Read bytes from a device.
 * Most I2C devices will auto-increment the register pointer internally allowing
 * you to read consecutive registers on a device in a single transaction.
 *
 * @param registerAddress The register to read first in the transaction.
 * @param count           The number of bytes to read in the transaction.
 * @param buffer          A pointer to the array of bytes to store the data
 *                        read from the device.
 * @return Transfer Aborted... false for success, true for aborted.
 */
bool I2C::Read(int registerAddress, int count, uint8_t* buffer) {
  if (count < 1) {
    wpi_setWPIErrorWithContext(ParameterOutOfRange, "count");
    return true;
  }
  if (buffer == nullptr) {
    wpi_setWPIErrorWithContext(NullParameter, "buffer");
    return true;
  }
  uint8_t regAddr = registerAddress;
  return Transaction(&regAddr, sizeof(regAddr), buffer, count);
}

/**
 * Execute a read only transaction with the device.
 *
 * Read bytes from a device. This method does not write any data to prompt the
 * device.
 *
 * @param buffer A pointer to the array of bytes to store the data read from
 *               the device.
 * @param count  The number of bytes to read in the transaction.
 * @return Transfer Aborted... false for success, true for aborted.
 */
bool I2C::ReadOnly(int count, uint8_t* buffer) {
  if (count < 1) {
    wpi_setWPIErrorWithContext(ParameterOutOfRange, "count");
    return true;
  }
  if (buffer == nullptr) {
    wpi_setWPIErrorWithContext(NullParameter, "buffer");
    return true;
  }
  return HAL_ReadI2C(m_port, m_deviceAddress, buffer, count) < 0;
}

/**
 * Send a broadcast write to all devices on the I2C bus.
 *
 * This is not currently implemented!
 *
 * @param registerAddress The register to write on all devices on the bus.
 * @param data            The value to write to the devices.
 */
// [[gnu::warning("I2C::Broadcast() is not implemented.")]] void I2C::Broadcast(
//     int registerAddress, uint8_t data) {}

/**
 * Verify that a device's registers contain expected values.
 *
 * Most devices will have a set of registers that contain a known value that
 * can be used to identify them.  This allows an I2C device driver to easily
 * verify that the device contains the expected value.
 *
 * @pre The device must support and be configured to use register
 * auto-increment.
 *
 * @param registerAddress The base register to start reading from the device.
 * @param count           The size of the field to be verified.
 * @param expected        A buffer containing the values expected from the
 *                        device.
 */
bool I2C::VerifySensor(int registerAddress, int count,
                       const uint8_t* expected) {
  // TODO: Make use of all 7 read bytes
  uint8_t deviceData[4];
  for (int i = 0, curRegisterAddress = registerAddress; i < count;
       i += 4, curRegisterAddress += 4) {
    int toRead = count - i < 4 ? count - i : 4;
    // Read the chunk of data.  Return false if the sensor does not respond.
    if (Read(curRegisterAddress, toRead, deviceData)) return false;

    for (int j = 0; j < toRead; j++) {
      if (deviceData[j] != expected[i + j]) return false;
    }
  }
  return true;
}
