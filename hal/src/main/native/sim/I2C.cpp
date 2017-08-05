/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/I2C.h"

extern "C" {
void HAL_InitializeI2C(HAL_I2CPort port, int32_t* status) {}
int32_t HAL_TransactionI2C(HAL_I2CPort port, int32_t deviceAddress,
                           uint8_t* dataToSend, int32_t sendSize,
                           uint8_t* dataReceived, int32_t receiveSize) {
  return 0;
}
int32_t HAL_WriteI2C(HAL_I2CPort port, int32_t deviceAddress,
                     uint8_t* dataToSend, int32_t sendSize) {
  return 0;
}
int32_t HAL_ReadI2C(HAL_I2CPort port, int32_t deviceAddress, uint8_t* buffer,
                    int32_t count) {
  return 0;
}
void HAL_CloseI2C(HAL_I2CPort port) {}
}
