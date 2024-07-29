// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/I2C.h"

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <cstring>

#include <wpi/print.h>

#include "DigitalInternal.h"
#include "HALInitializer.h"
#include "HALInternal.h"
#include "hal/DIO.h"
#include "hal/HAL.h"

using namespace hal;

static wpi::mutex digitalI2COnBoardMutex;
static wpi::mutex digitalI2CMXPMutex;

static uint8_t i2COnboardObjCount{0};
static uint8_t i2CMXPObjCount{0};
static int i2COnBoardHandle{-1};
static int i2CMXPHandle{-1};

static HAL_DigitalHandle i2CMXPDigitalHandle1{HAL_kInvalidHandle};
static HAL_DigitalHandle i2CMXPDigitalHandle2{HAL_kInvalidHandle};

namespace hal::init {
void InitializeI2C() {}
}  // namespace hal::init

extern "C" {

void HAL_InitializeI2C(HAL_I2CPort port, int32_t* status) {
  hal::init::CheckInit();
  initializeDigital(status);
  if (*status != 0) {
    return;
  }

  if (port < 0 || port > 1) {
    *status = RESOURCE_OUT_OF_RANGE;
    hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for I2C", 0, 1,
                                     port);
    return;
  }

  if (port == HAL_I2C_kOnboard) {
    HAL_SendError(0, 0, 0,
                  "Onboard I2C port is subject to system lockups. See Known "
                  "Issues page for details",
                  "", "", true);
    std::scoped_lock lock(digitalI2COnBoardMutex);
    i2COnboardObjCount++;
    if (i2COnboardObjCount > 1) {
      return;
    }
    int handle = open("/dev/i2c-2", O_RDWR);
    if (handle < 0) {
      wpi::print("Failed to open onboard i2c bus: {}\n", std::strerror(errno));
      return;
    }
    i2COnBoardHandle = handle;
  } else {
    std::scoped_lock lock(digitalI2CMXPMutex);
    i2CMXPObjCount++;
    if (i2CMXPObjCount > 1) {
      return;
    }
    if ((i2CMXPDigitalHandle1 = HAL_InitializeDIOPort(
             HAL_GetPort(24), false, nullptr, status)) == HAL_kInvalidHandle) {
      return;
    }
    if ((i2CMXPDigitalHandle2 = HAL_InitializeDIOPort(
             HAL_GetPort(25), false, nullptr, status)) == HAL_kInvalidHandle) {
      HAL_FreeDIOPort(i2CMXPDigitalHandle1);  // free the first port allocated
      return;
    }
    digitalSystem->writeEnableMXPSpecialFunction(
        digitalSystem->readEnableMXPSpecialFunction(status) | 0xC000, status);
    int handle = open("/dev/i2c-1", O_RDWR);
    if (handle < 0) {
      wpi::print("Failed to open MXP i2c bus: {}\n", std::strerror(errno));
      return;
    }
    i2CMXPHandle = handle;
  }
}

int32_t HAL_TransactionI2C(HAL_I2CPort port, int32_t deviceAddress,
                           const uint8_t* dataToSend, int32_t sendSize,
                           uint8_t* dataReceived, int32_t receiveSize) {
  if (port < 0 || port > 1) {
    int32_t status = 0;
    hal::SetLastErrorIndexOutOfRange(&status, "Invalid Index for I2C", 0, 1,
                                     port);
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

  if (port == HAL_I2C_kOnboard) {
    std::scoped_lock lock(digitalI2COnBoardMutex);
    return ioctl(i2COnBoardHandle, I2C_RDWR, &rdwr);
  } else {
    std::scoped_lock lock(digitalI2CMXPMutex);
    return ioctl(i2CMXPHandle, I2C_RDWR, &rdwr);
  }
}

int32_t HAL_WriteI2C(HAL_I2CPort port, int32_t deviceAddress,
                     const uint8_t* dataToSend, int32_t sendSize) {
  if (port < 0 || port > 1) {
    int32_t status = 0;
    hal::SetLastErrorIndexOutOfRange(&status, "Invalid Index for I2C", 0, 1,
                                     port);
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

  if (port == HAL_I2C_kOnboard) {
    std::scoped_lock lock(digitalI2COnBoardMutex);
    return ioctl(i2COnBoardHandle, I2C_RDWR, &rdwr);
  } else {
    std::scoped_lock lock(digitalI2CMXPMutex);
    return ioctl(i2CMXPHandle, I2C_RDWR, &rdwr);
  }
}

int32_t HAL_ReadI2C(HAL_I2CPort port, int32_t deviceAddress, uint8_t* buffer,
                    int32_t count) {
  if (port < 0 || port > 1) {
    int32_t status = 0;
    hal::SetLastErrorIndexOutOfRange(&status, "Invalid Index for I2C", 0, 1,
                                     port);
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

  if (port == HAL_I2C_kOnboard) {
    std::scoped_lock lock(digitalI2COnBoardMutex);
    return ioctl(i2COnBoardHandle, I2C_RDWR, &rdwr);
  } else {
    std::scoped_lock lock(digitalI2CMXPMutex);
    return ioctl(i2CMXPHandle, I2C_RDWR, &rdwr);
  }
}

void HAL_CloseI2C(HAL_I2CPort port) {
  if (port < 0 || port > 1) {
    int32_t status = 0;
    hal::SetLastErrorIndexOutOfRange(&status, "Invalid Index for I2C", 0, 1,
                                     port);
    return;
  }

  if (port == HAL_I2C_kOnboard) {
    std::scoped_lock lock(digitalI2COnBoardMutex);
    if (i2COnboardObjCount-- == 0) {
      close(i2COnBoardHandle);
    }
  } else {
    std::scoped_lock lock(digitalI2CMXPMutex);
    if (i2CMXPObjCount-- == 0) {
      close(i2CMXPHandle);
    }
    HAL_FreeDIOPort(i2CMXPDigitalHandle1);
    HAL_FreeDIOPort(i2CMXPDigitalHandle2);
  }
}

}  // extern "C"
