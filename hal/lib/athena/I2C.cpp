/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/I2C.h"

#include "DigitalInternal.h"
#include "HAL/DIO.h"
#include "HAL/HAL.h"
#include "i2clib/i2c-lib.h"

static_assert(sizeof(uint32_t) <= sizeof(void*),
              "This file shoves uint32_ts into pointers.");

using namespace hal;

static priority_recursive_mutex digitalI2COnBoardMutex;
static priority_recursive_mutex digitalI2CMXPMutex;

static uint8_t i2COnboardObjCount = 0;
static uint8_t i2CMXPObjCount = 0;
static uint8_t i2COnBoardHandle = 0;
static uint8_t i2CMXPHandle = 0;

extern "C" {
/*
 * Initialize the I2C port. Opens the port if necessary and saves the handle.
 * If opening the MXP port, also sets up the pin functions appropriately
 * @param port The port to open, 0 for the on-board, 1 for the MXP.
 */
void i2CInitialize(uint8_t port, int32_t* status) {
  initializeDigital(status);
  if (*status != 0) return;

  if (port > 1) {
    // Set port out of range error here
    return;
  }

  priority_recursive_mutex& lock =
      port == 0 ? digitalI2COnBoardMutex : digitalI2CMXPMutex;
  {
    std::lock_guard<priority_recursive_mutex> sync(lock);
    if (port == 0) {
      i2COnboardObjCount++;
      if (i2COnBoardHandle > 0) return;
      i2COnBoardHandle = i2clib_open("/dev/i2c-2");
    } else if (port == 1) {
      i2CMXPObjCount++;
      if (i2CMXPHandle > 0) return;
      if (!allocateDIO(initializeDigitalPort(getPort(24), status), false,
                       status))
        return;
      if (!allocateDIO(initializeDigitalPort(getPort(25), status), false,
                       status))
        return;
      digitalSystem->writeEnableMXPSpecialFunction(
          digitalSystem->readEnableMXPSpecialFunction(status) | 0xC000, status);
      i2CMXPHandle = i2clib_open("/dev/i2c-1");
    }
    return;
  }
}

/**
 * Generic transaction.
 *
 * This is a lower-level interface to the I2C hardware giving you more control
 * over each transaction.
 *
 * @param dataToSend Buffer of data to send as part of the transaction.
 * @param sendSize Number of bytes to send as part of the transaction.
 * @param dataReceived Buffer to read data into.
 * @param receiveSize Number of bytes to read from the device.
 * @return The number of bytes read (>= 0) or -1 on transfer abort.
 */
int32_t i2CTransaction(uint8_t port, uint8_t deviceAddress, uint8_t* dataToSend,
                       uint8_t sendSize, uint8_t* dataReceived,
                       uint8_t receiveSize) {
  if (port > 1) {
    // Set port out of range error here
    return -1;
  }

  int32_t handle = port == 0 ? i2COnBoardHandle : i2CMXPHandle;
  priority_recursive_mutex& lock =
      port == 0 ? digitalI2COnBoardMutex : digitalI2CMXPMutex;

  {
    std::lock_guard<priority_recursive_mutex> sync(lock);
    return i2clib_writeread(handle, deviceAddress, (const char*)dataToSend,
                            (int32_t)sendSize, (char*)dataReceived,
                            (int32_t)receiveSize);
  }
}

/**
 * Execute a write transaction with the device.
 *
 * Write a single byte to a register on a device and wait until the
 *   transaction is complete.
 *
 * @param registerAddress The address of the register on the device to be
 * written.
 * @param data The byte to write to the register on the device.
 * @return The number of bytes written (>= 0) or -1 on transfer abort.
 */
int32_t i2CWrite(uint8_t port, uint8_t deviceAddress, uint8_t* dataToSend,
                 uint8_t sendSize) {
  if (port > 1) {
    // Set port out of range error here
    return -1;
  }

  int32_t handle = port == 0 ? i2COnBoardHandle : i2CMXPHandle;
  priority_recursive_mutex& lock =
      port == 0 ? digitalI2COnBoardMutex : digitalI2CMXPMutex;
  {
    std::lock_guard<priority_recursive_mutex> sync(lock);
    return i2clib_write(handle, deviceAddress, (const char*)dataToSend,
                        (int32_t)sendSize);
  }
}

/**
 * Execute a read transaction with the device.
 *
 * Read bytes from a device.
 * Most I2C devices will auto-increment the register pointer internally allowing
 *   you to read consecutive registers on a device in a single transaction.
 *
 * @param registerAddress The register to read first in the transaction.
 * @param count The number of bytes to read in the transaction.
 * @param buffer A pointer to the array of bytes to store the data read from the
 * device.
 * @return The number of bytes read (>= 0) or -1 on transfer abort.
 */
int32_t i2CRead(uint8_t port, uint8_t deviceAddress, uint8_t* buffer,
                uint8_t count) {
  if (port > 1) {
    // Set port out of range error here
    return -1;
  }

  int32_t handle = port == 0 ? i2COnBoardHandle : i2CMXPHandle;
  priority_recursive_mutex& lock =
      port == 0 ? digitalI2COnBoardMutex : digitalI2CMXPMutex;
  {
    std::lock_guard<priority_recursive_mutex> sync(lock);
    return i2clib_read(handle, deviceAddress, (char*)buffer, (int32_t)count);
  }
}

void i2CClose(uint8_t port) {
  if (port > 1) {
    // Set port out of range error here
    return;
  }
  priority_recursive_mutex& lock =
      port == 0 ? digitalI2COnBoardMutex : digitalI2CMXPMutex;
  {
    std::lock_guard<priority_recursive_mutex> sync(lock);
    if ((port == 0 ? i2COnboardObjCount-- : i2CMXPObjCount--) == 0) {
      int32_t handle = port == 0 ? i2COnBoardHandle : i2CMXPHandle;
      i2clib_close(handle);
    }
  }
  return;
}
}
