/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
void HAL_InitializeSerialPort(int32_t port, int32_t* status);
void HAL_SetSerialBaudRate(int32_t port, int32_t baud, int32_t* status);
void HAL_SetSerialDataBits(int32_t port, int32_t bits, int32_t* status);
void HAL_SetSerialParity(int32_t port, int32_t parity, int32_t* status);
void HAL_SetSerialStopBits(int32_t port, int32_t stopBits, int32_t* status);
void HAL_SetSerialWriteMode(int32_t port, int32_t mode, int32_t* status);
void HAL_SetSerialFlowControl(int32_t port, int32_t flow, int32_t* status);
void HAL_SetSerialTimeout(int32_t port, double timeout, int32_t* status);
void HAL_EnableSerialTermination(int32_t port, char terminator,
                                 int32_t* status);
void HAL_DisableSerialTermination(int32_t port, int32_t* status);
void HAL_SetSerialReadBufferSize(int32_t port, int32_t size, int32_t* status);
void HAL_SetSerialWriteBufferSize(int32_t port, int32_t size, int32_t* status);
int32_t HAL_GetSerialBytesReceived(int32_t port, int32_t* status);
int32_t HAL_ReadSerial(int32_t port, char* buffer, int32_t count,
                       int32_t* status);
int32_t HAL_WriteSerial(int32_t port, const char* buffer, int32_t count,
                        int32_t* status);
void HAL_FlushSerial(int32_t port, int32_t* status);
void HAL_ClearSerial(int32_t port, int32_t* status);
void HAL_CloseSerial(int32_t port, int32_t* status);
#ifdef __cplusplus
}
#endif
