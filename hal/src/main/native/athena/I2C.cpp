/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/I2C.h"

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <cstring>

#include "DigitalInternal.h"
#include "HAL/DIO.h"
#include "HAL/HAL.h"

using namespace hal;

static wpi::mutex digitalI2COnBoardMutex;
static wpi::mutex digitalI2CMXPMutex;

static uint8_t i2COnboardObjCount{0};
static uint8_t i2CMXPObjCount{0};
static int i2COnBoardHandle{-1};
static int i2CMXPHandle{-1};

static HAL_DigitalHandle i2CMXPDigitalHandle1{HAL_kInvalidHandle};
static HAL_DigitalHandle i2CMXPDigitalHandle2{HAL_kInvalidHandle};

namespace hal {
namespace init {
void InitializeI2C() {}
}  // namespace init
}  // namespace hal

extern "C" {

/*
 * Initialize the I2C port. Opens the port if necessary and saves the handle.
 * If opening the MXP port, also sets up the channel functions appropriately
 * @param port The port to open, 0 for the on-board, 1 for the MXP.
 */
void HAL_InitializeI2C(HAL_I2CPort port, int32_t* status) {
  initializeDigital(status);
  if (*status != 0) return;

  if (port > 1) {
    // Set port out of range error here
    return;
  }

  if (port == 0) {
    std::lock_guard<wpi::mutex> lock(digitalI2COnBoardMutex);
    i2COnboardObjCount++;
    if (i2COnboardObjCount > 1) return;
    int handle = open("/dev/i2c-2", O_RDWR);
    if (handle < 0) {
      std::printf("Failed to open onboard i2c bus: %s\n", std::strerror(errno));
      return;
    }
    i2COnBoardHandle = handle;
  } else {
    std::lock_guard<wpi::mutex> lock(digitalI2CMXPMutex);
    i2CMXPObjCount++;
    if (i2CMXPObjCount > 1) return;
    if ((i2CMXPDigitalHandle1 = HAL_InitializeDIOPort(
             HAL_GetPort(24), false, status)) == HAL_kInvalidHandle) {
      return;
    }
    if ((i2CMXPDigitalHandle2 = HAL_InitializeDIOPort(
             HAL_GetPort(25), false, status)) == HAL_kInvalidHandle) {
      HAL_FreeDIOPort(i2CMXPDigitalHandle1);  // free the first port allocated
      return;
    }
    digitalSystem->writeEnableMXPSpecialFunction(
        digitalSystem->readEnableMXPSpecialFunction(status) | 0xC000, status);
    int handle = open("/dev/i2c-1", O_RDWR);
    if (handle < 0) {
      std::printf("Failed to open MXP i2c bus: %s\n", std::strerror(errno));
      return;
    }
    i2CMXPHandle = handle;
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
 * @return >= 0 on success or -1 on transfer abort.
 */
int32_t HAL_TransactionI2C(HAL_I2CPort port, int32_t deviceAddress,
                           const uint8_t* dataToSend, int32_t sendSize,
                           uint8_t* dataReceived, int32_t receiveSize) {
  if (port > 1) {
    // Set port out of range error here
    return -1;
  }

  struct i2c_msg msgs[2];
  msgs[0].addr = deviceAddress;
  msgs[0].flags = 0;
  msgs[0].len = sendSize;
  msgs[0].buf = const_cast<uint8_t*>(dataToSend);
  msgs[1].addr = deviceAddress;
  msgs[1].flags = I2C_M_RD;
  msgs[1].len = receiveSize;
  msgs[1].buf = dataReceived;

  struct i2c_rdwr_ioctl_data rdwr;
  rdwr.msgs = msgs;
  rdwr.nmsgs = 2;

  if (port == 0) {
    std::lock_guard<wpi::mutex> lock(digitalI2COnBoardMutex);
    return ioctl(i2COnBoardHandle, I2C_RDWR, &rdwr);
  } else {
    std::lock_guard<wpi::mutex> lock(digitalI2CMXPMutex);
    return ioctl(i2CMXPHandle, I2C_RDWR, &rdwr);
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
 * @return >= 0 on success or -1 on transfer abort.
 */
int32_t HAL_WriteI2C(HAL_I2CPort port, int32_t deviceAddress,
                     const uint8_t* dataToSend, int32_t sendSize) {
  if (port > 1) {
    // Set port out of range error here
    return -1;
  }

  struct i2c_msg msg;
  msg.addr = deviceAddress;
  msg.flags = 0;
  msg.len = sendSize;
  msg.buf = const_cast<uint8_t*>(dataToSend);

  struct i2c_rdwr_ioctl_data rdwr;
  rdwr.msgs = &msg;
  rdwr.nmsgs = 1;

  if (port == 0) {
    std::lock_guard<wpi::mutex> lock(digitalI2COnBoardMutex);
    return ioctl(i2COnBoardHandle, I2C_RDWR, &rdwr);
  } else {
    std::lock_guard<wpi::mutex> lock(digitalI2CMXPMutex);
    return ioctl(i2CMXPHandle, I2C_RDWR, &rdwr);
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
 * @return >= 0 on success or -1 on transfer abort.
 */
int32_t HAL_ReadI2C(HAL_I2CPort port, int32_t deviceAddress, uint8_t* buffer,
                    int32_t count) {
  if (port > 1) {
    // Set port out of range error here
    return -1;
  }

  struct i2c_msg msg;
  msg.addr = deviceAddress;
  msg.flags = I2C_M_RD;
  msg.len = count;
  msg.buf = buffer;

  struct i2c_rdwr_ioctl_data rdwr;
  rdwr.msgs = &msg;
  rdwr.nmsgs = 1;

  if (port == 0) {
    std::lock_guard<wpi::mutex> lock(digitalI2COnBoardMutex);
    return ioctl(i2COnBoardHandle, I2C_RDWR, &rdwr);
  } else {
    std::lock_guard<wpi::mutex> lock(digitalI2CMXPMutex);
    return ioctl(i2CMXPHandle, I2C_RDWR, &rdwr);
  }
}

void HAL_CloseI2C(HAL_I2CPort port) {
  if (port > 1) {
    // Set port out of range error here
    return;
  }

  if (port == 0) {
    std::lock_guard<wpi::mutex> lock(digitalI2COnBoardMutex);
    if (i2COnboardObjCount-- == 0) {
      close(i2COnBoardHandle);
    }
  } else {
    std::lock_guard<wpi::mutex> lock(digitalI2CMXPMutex);
    if (i2CMXPObjCount-- == 0) {
      close(i2CMXPHandle);
    }
    HAL_FreeDIOPort(i2CMXPDigitalHandle1);
    HAL_FreeDIOPort(i2CMXPDigitalHandle2);
  }
}

}  // extern "C"
