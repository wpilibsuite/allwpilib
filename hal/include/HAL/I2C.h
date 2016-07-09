/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

extern "C" {
void HAL_InitializeI2C(uint8_t port, int32_t* status);
int32_t HAL_TransactionI2C(uint8_t port, uint8_t deviceAddress,
                           uint8_t* dataToSend, uint8_t sendSize,
                           uint8_t* dataReceived, uint8_t receiveSize);
int32_t HAL_WriteI2C(uint8_t port, uint8_t deviceAddress, uint8_t* dataToSend,
                     uint8_t sendSize);
int32_t HAL_ReadI2C(uint8_t port, uint8_t deviceAddress, uint8_t* buffer,
                    uint8_t count);
void HAL_CloseI2C(uint8_t port);
}
