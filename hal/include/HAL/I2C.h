/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void HAL_InitializeI2C(int32_t port, int32_t* status);
int32_t HAL_TransactionI2C(int32_t port, int32_t deviceAddress,
                           uint8_t* dataToSend, int32_t sendSize,
                           uint8_t* dataReceived, int32_t receiveSize);
int32_t HAL_WriteI2C(int32_t port, int32_t deviceAddress, uint8_t* dataToSend,
                     int32_t sendSize);
int32_t HAL_ReadI2C(int32_t port, int32_t deviceAddress, uint8_t* buffer,
                    int32_t count);
void HAL_CloseI2C(int32_t port);
#ifdef __cplusplus
}
#endif
