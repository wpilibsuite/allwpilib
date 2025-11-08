// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/I2C.h"

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <cstring>

#include <fmt/format.h>

#include "HALInitializer.h"
#include "HALInternal.h"
#include "PortsInternal.h"
#include "wpi/hal/DIO.h"
#include "wpi/hal/HAL.h"
#include "wpi/util/mutex.hpp"
#include "wpi/util/print.hpp"

using namespace wpi::hal;

namespace {
constexpr const char* physicalPorts[kNumI2cBuses] = {"/dev/i2c-10",
                                                     "/dev/i2c-1"};

struct I2C {
  wpi::util::mutex initMutex;
  int objCount = 0;
  int fd = -1;
};

static I2C i2cObjs[kNumI2cBuses];
}  // namespace

namespace wpi::hal::init {
void InitializeI2C() {}
}  // namespace wpi::hal::init

extern "C" {
void HAL_InitializeI2C(HAL_I2CPort port, int32_t* status) {
  wpi::hal::init::CheckInit();

  if (port < 0 || port > 2) {
    *status = RESOURCE_OUT_OF_RANGE;
    wpi::hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for I2C", 0, 1,
                                          port);
    return;
  }

  std::scoped_lock lock(i2cObjs[port].initMutex);
  i2cObjs[port].objCount++;
  if (i2cObjs[port].objCount > 1) {
    return;
  }
  int handle = open(physicalPorts[port], O_RDWR);
  if (handle < 0) {
    int err = errno;
    *status = NO_AVAILABLE_RESOURCES;
    wpi::hal::SetLastError(
        status,
        fmt::format("Failed to open onboard i2c bus: {}", std::strerror(err)));
    wpi::util::print("Failed to open onboard i2c bus: {}\n",
                     std::strerror(err));
    handle = -1;
    i2cObjs[port].objCount--;
    return;
  }
  i2cObjs[port].fd = handle;
}

int32_t HAL_TransactionI2C(HAL_I2CPort port, int32_t deviceAddress,
                           const uint8_t* dataToSend, int32_t sendSize,
                           uint8_t* dataReceived, int32_t receiveSize) {
  if (port < 0 || port > 2) {
    int32_t status = 0;
    wpi::hal::SetLastErrorIndexOutOfRange(&status, "Invalid Index for I2C", 0,
                                          1, port);
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

  std::scoped_lock lock(i2cObjs[port].initMutex);
  return ioctl(i2cObjs[port].fd, I2C_RDWR, &rdwr);
}

int32_t HAL_WriteI2C(HAL_I2CPort port, int32_t deviceAddress,
                     const uint8_t* dataToSend, int32_t sendSize) {
  if (port < 0 || port > 2) {
    int32_t status = 0;
    wpi::hal::SetLastErrorIndexOutOfRange(&status, "Invalid Index for I2C", 0,
                                          2, port);
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

  std::scoped_lock lock(i2cObjs[port].initMutex);
  return ioctl(i2cObjs[port].fd, I2C_RDWR, &rdwr);
}

int32_t HAL_ReadI2C(HAL_I2CPort port, int32_t deviceAddress, uint8_t* buffer,
                    int32_t count) {
  if (port < 0 || port > 2) {
    int32_t status = 0;
    wpi::hal::SetLastErrorIndexOutOfRange(&status, "Invalid Index for I2C", 0,
                                          1, port);
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

  std::scoped_lock lock(i2cObjs[port].initMutex);
  return ioctl(i2cObjs[port].fd, I2C_RDWR, &rdwr);
}

void HAL_CloseI2C(HAL_I2CPort port) {
  if (port < 0 || port > 2) {
    int32_t status = 0;
    wpi::hal::SetLastErrorIndexOutOfRange(&status, "Invalid Index for I2C", 0,
                                          1, port);
    return;
  }

  std::scoped_lock lock(i2cObjs[port].initMutex);
  if (i2cObjs[port].objCount-- == 0) {
    close(i2cObjs[port].objCount);
    i2cObjs[port].fd = -1;
  }
}

}  // extern "C"
