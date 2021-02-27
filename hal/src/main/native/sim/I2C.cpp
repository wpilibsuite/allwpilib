// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/I2C.h"

#include "HALInitializer.h"
#include "mockdata/I2CDataInternal.h"

using namespace hal;

namespace hal::init {
void InitializeI2C() {}
}  // namespace hal::init

extern "C" {
void HAL_InitializeI2C(HAL_I2CPort port, int32_t* status) {
  hal::init::CheckInit();
  SimI2CData[port].initialized = true;
}
int32_t HAL_TransactionI2C(HAL_I2CPort port, int32_t deviceAddress,
                           const uint8_t* dataToSend, int32_t sendSize,
                           uint8_t* dataReceived, int32_t receiveSize) {
  SimI2CData[port].Write(deviceAddress, dataToSend, sendSize);
  SimI2CData[port].Read(deviceAddress, dataReceived, receiveSize);
  return 0;
}
int32_t HAL_WriteI2C(HAL_I2CPort port, int32_t deviceAddress,
                     const uint8_t* dataToSend, int32_t sendSize) {
  SimI2CData[port].Write(deviceAddress, dataToSend, sendSize);
  return 0;
}
int32_t HAL_ReadI2C(HAL_I2CPort port, int32_t deviceAddress, uint8_t* buffer,
                    int32_t count) {
  SimI2CData[port].Read(deviceAddress, buffer, count);
  return 0;
}
void HAL_CloseI2C(HAL_I2CPort port) {
  SimI2CData[port].initialized = false;
}
}  // extern "C"
