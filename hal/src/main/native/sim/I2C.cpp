/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/I2C.h"

#include "HALInitializer.h"
#include "mockdata/I2CDataInternal.h"

using namespace hal;

namespace hal {
namespace init {
void InitializeI2C() {}
}  // namespace init
}  // namespace hal

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
void HAL_CloseI2C(HAL_I2CPort port) { SimI2CData[port].initialized = false; }
}  // extern "C"
