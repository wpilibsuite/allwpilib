/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

extern "C" {
void HAL_SerialInitializePort(uint8_t port, int32_t* status);
void HAL_SerialSetBaudRate(uint8_t port, uint32_t baud, int32_t* status);
void HAL_SerialSetDataBits(uint8_t port, uint8_t bits, int32_t* status);
void HAL_SerialSetParity(uint8_t port, uint8_t parity, int32_t* status);
void HAL_SerialSetStopBits(uint8_t port, uint8_t stopBits, int32_t* status);
void HAL_SerialSetWriteMode(uint8_t port, uint8_t mode, int32_t* status);
void HAL_SerialSetFlowControl(uint8_t port, uint8_t flow, int32_t* status);
void HAL_SerialSetTimeout(uint8_t port, float timeout, int32_t* status);
void HAL_SerialEnableTermination(uint8_t port, char terminator,
                                 int32_t* status);
void HAL_SerialDisableTermination(uint8_t port, int32_t* status);
void HAL_SerialSetReadBufferSize(uint8_t port, uint32_t size, int32_t* status);
void HAL_SerialSetWriteBufferSize(uint8_t port, uint32_t size, int32_t* status);
int32_t HAL_SerialGetBytesReceived(uint8_t port, int32_t* status);
uint32_t HAL_SerialRead(uint8_t port, char* buffer, int32_t count,
                        int32_t* status);
uint32_t HAL_SerialWrite(uint8_t port, const char* buffer, int32_t count,
                         int32_t* status);
void HAL_SerialFlush(uint8_t port, int32_t* status);
void HAL_SerialClear(uint8_t port, int32_t* status);
void HAL_SerialClose(uint8_t port, int32_t* status);
}
