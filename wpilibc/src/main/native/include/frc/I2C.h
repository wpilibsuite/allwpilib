// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <hal/I2C.h>
#include <hal/I2CTypes.h>

namespace frc {

/**
 * I2C bus interface class.
 *
 * This class is intended to be used by sensor (and other I2C device) drivers.
 * It probably should not be used directly.
 */
class I2C {
 public:
  /**
   * I2C connection ports.
   */
  enum Port {
    /// I2C Port 0.
    kPort0 = 0,
    /// I2C Port 1.
    kPort1
  };

  /**
   * Constructor.
   *
   * @param port          The I2C port to which the device is connected.
   * @param deviceAddress The address of the device on the I2C bus.
   */
  I2C(Port port, int deviceAddress);

  I2C(I2C&&) = default;
  I2C& operator=(I2C&&) = default;

  /**
   * Returns I2C port.
   *
   * @return I2C port.
   */
  Port GetPort() const;

  /**
   * Returns I2C device address.
   *
   * @return I2C device address.
   */
  int GetDeviceAddress() const;

  /**
   * Generic transaction.
   *
   * This is a lower-level interface to the I2C hardware giving you more control
   * over each transaction. If you intend to write multiple bytes in the same
   * transaction and do not plan to receive anything back, use writeBulk()
   * instead. Calling this with a receiveSize of 0 will result in an error.
   *
   * @param dataToSend   Buffer of data to send as part of the transaction.
   * @param sendSize     Number of bytes to send as part of the transaction.
   * @param dataReceived Buffer to read data into.
   * @param receiveSize  Number of bytes to read from the device.
   * @return Transfer Aborted... false for success, true for aborted.
   */
  bool Transaction(uint8_t* dataToSend, int sendSize, uint8_t* dataReceived,
                   int receiveSize);

  /**
   * Attempt to address a device on the I2C bus.
   *
   * This allows you to figure out if there is a device on the I2C bus that
   * responds to the address specified in the constructor.
   *
   * @return Transfer Aborted... false for success, true for aborted.
   */
  bool AddressOnly();

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
  bool Write(int registerAddress, uint8_t data);

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
  bool WriteBulk(uint8_t* data, int count);

  /**
   * Execute a read transaction with the device.
   *
   * Read bytes from a device.
   * Most I2C devices will auto-increment the register pointer internally
   * allowing you to read consecutive registers on a device in a single
   * transaction.
   *
   * @param registerAddress The register to read first in the transaction.
   * @param count           The number of bytes to read in the transaction.
   * @param data            A pointer to the array of bytes to store the data
   *                        read from the device.
   * @return Transfer Aborted... false for success, true for aborted.
   */
  bool Read(int registerAddress, int count, uint8_t* data);

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
  bool ReadOnly(int count, uint8_t* buffer);

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
  bool VerifySensor(int registerAddress, int count, const uint8_t* expected);

 private:
  hal::Handle<HAL_I2CPort, HAL_CloseI2C, HAL_I2C_kInvalid> m_port;
  int m_deviceAddress;
};

}  // namespace frc
