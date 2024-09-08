// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/SerialPort.h"

#include "HALInitializer.h"

namespace hal::init {
void InitializeSerialPort() {}
}  // namespace hal::init

extern "C" {
HAL_SerialPortHandle HAL_InitializeSerialPort(HAL_SerialPort port,
                                              int32_t* status) {
  hal::init::CheckInit();
  return HAL_kInvalidHandle;
}

HAL_SerialPortHandle HAL_InitializeSerialPortDirect(HAL_SerialPort port,
                                                    const char* portName,
                                                    int32_t* status) {
  hal::init::CheckInit();
  return HAL_kInvalidHandle;
}

int HAL_GetSerialFD(HAL_SerialPortHandle handle, int32_t* status) {
  return -1;
}

void HAL_SetSerialBaudRate(HAL_SerialPortHandle handle, int32_t baud,
                           int32_t* status) {}

void HAL_SetSerialDataBits(HAL_SerialPortHandle handle, int32_t bits,
                           int32_t* status) {}

void HAL_SetSerialParity(HAL_SerialPortHandle handle, int32_t parity,
                         int32_t* status) {}

void HAL_SetSerialStopBits(HAL_SerialPortHandle handle, int32_t stopBits,
                           int32_t* status) {}

void HAL_SetSerialWriteMode(HAL_SerialPortHandle handle, int32_t mode,
                            int32_t* status) {}

void HAL_SetSerialFlowControl(HAL_SerialPortHandle handle, int32_t flow,
                              int32_t* status) {}

void HAL_SetSerialTimeout(HAL_SerialPortHandle handle, double timeout,
                          int32_t* status) {}

void HAL_EnableSerialTermination(HAL_SerialPortHandle handle, char terminator,
                                 int32_t* status) {}

void HAL_DisableSerialTermination(HAL_SerialPortHandle handle,
                                  int32_t* status) {}

void HAL_SetSerialReadBufferSize(HAL_SerialPortHandle handle, int32_t size,
                                 int32_t* status) {}

void HAL_SetSerialWriteBufferSize(HAL_SerialPortHandle handle, int32_t size,
                                  int32_t* status) {}

int32_t HAL_GetSerialBytesReceived(HAL_SerialPortHandle handle,
                                   int32_t* status) {
  return 0;
}

int32_t HAL_ReadSerial(HAL_SerialPortHandle handle, char* buffer, int32_t count,
                       int32_t* status) {
  return 0;
}

int32_t HAL_WriteSerial(HAL_SerialPortHandle handle, const char* buffer,
                        int32_t count, int32_t* status) {
  return 0;
}

void HAL_FlushSerial(HAL_SerialPortHandle handle, int32_t* status) {}

void HAL_ClearSerial(HAL_SerialPortHandle handle, int32_t* status) {}

void HAL_CloseSerial(HAL_SerialPortHandle handle) {}
}  // extern "C"
