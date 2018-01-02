/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "SerialPort.h"

#ifdef __cplusplus
extern "C" {
#endif

void HAL_InitializeOSSerialPort(HAL_SerialPort port, int32_t* status);
void HAL_SetOSSerialBaudRate(HAL_SerialPort port, int32_t baud,
                             int32_t* status);
void HAL_SetOSSerialDataBits(HAL_SerialPort port, int32_t bits,
                             int32_t* status);
void HAL_SetOSSerialParity(HAL_SerialPort port, int32_t parity,
                           int32_t* status);
void HAL_SetOSSerialStopBits(HAL_SerialPort port, int32_t stopBits,
                             int32_t* status);
void HAL_SetOSSerialWriteMode(HAL_SerialPort port, int32_t mode,
                              int32_t* status);
void HAL_SetOSSerialFlowControl(HAL_SerialPort port, int32_t flow,
                                int32_t* status);
void HAL_SetOSSerialTimeout(HAL_SerialPort port, double timeout,
                            int32_t* status);
void HAL_EnableOSSerialTermination(HAL_SerialPort port, char terminator,
                                   int32_t* status);
void HAL_DisableOSSerialTermination(HAL_SerialPort port, int32_t* status);
void HAL_SetOSSerialReadBufferSize(HAL_SerialPort port, int32_t size,
                                   int32_t* status);
void HAL_SetOSSerialWriteBufferSize(HAL_SerialPort port, int32_t size,
                                    int32_t* status);
int32_t HAL_GetOSSerialBytesReceived(HAL_SerialPort port, int32_t* status);
int32_t HAL_ReadOSSerial(HAL_SerialPort port, char* buffer, int32_t count,
                         int32_t* status);
int32_t HAL_WriteOSSerial(HAL_SerialPort port, const char* buffer,
                          int32_t count, int32_t* status);
void HAL_FlushOSSerial(HAL_SerialPort port, int32_t* status);
void HAL_ClearOSSerial(HAL_SerialPort port, int32_t* status);
void HAL_CloseOSSerial(HAL_SerialPort port, int32_t* status);
#ifdef __cplusplus
}  // extern "C"
#endif
