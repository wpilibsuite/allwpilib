/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

extern "C" {
void HAL_InitializeSerialPort(uint8_t port, int32_t* status);
void HAL_SetSerialBaudRate(uint8_t port, uint32_t baud, int32_t* status);
void HAL_SetSerialDataBits(uint8_t port, uint8_t bits, int32_t* status);
void HAL_SetSerialParity(uint8_t port, uint8_t parity, int32_t* status);
void HAL_SetSerialStopBits(uint8_t port, uint8_t stopBits, int32_t* status);
void HAL_SetSerialWriteMode(uint8_t port, uint8_t mode, int32_t* status);
void HAL_SetSerialFlowControl(uint8_t port, uint8_t flow, int32_t* status);
void HAL_SetSerialTimeout(uint8_t port, float timeout, int32_t* status);
void HAL_EnableSerialTermination(uint8_t port, char terminator,
                                 int32_t* status);
void HAL_DisableSerialTermination(uint8_t port, int32_t* status);
void HAL_SetSerialReadBufferSize(uint8_t port, uint32_t size, int32_t* status);
void HAL_SetSerialWriteBufferSize(uint8_t port, uint32_t size, int32_t* status);
int32_t HAL_GetSerialBytesReceived(uint8_t port, int32_t* status);
uint32_t HAL_ReadSerial(uint8_t port, char* buffer, int32_t count,
                        int32_t* status);
uint32_t HAL_WriteSerial(uint8_t port, const char* buffer, int32_t count,
                         int32_t* status);
void HAL_FlushSerial(uint8_t port, int32_t* status);
void HAL_ClearSerial(uint8_t port, int32_t* status);
void HAL_CloseSerial(uint8_t port, int32_t* status);
}
