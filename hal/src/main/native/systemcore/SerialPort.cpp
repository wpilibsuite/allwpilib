// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/SerialPort.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include <cerrno>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>
#include <thread>

#include <fmt/format.h>

#include "HALInternal.h"
#include "hal/cpp/SerialHelper.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/handles/IndexedHandleResource.h"

namespace hal::init {
void InitializeSerialPort() {}
}  // namespace hal::init

using namespace hal;

extern "C" {
HAL_SerialPortHandle HAL_InitializeSerialPort(HAL_SerialPort port,
                                              int32_t* status) {
  // hal::init::CheckInit();

  *status = HAL_HANDLE_ERROR;
  return HAL_kInvalidHandle;
}
HAL_SerialPortHandle HAL_InitializeSerialPortDirect(HAL_SerialPort port,
                                                    const char* portName,
                                                    int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return HAL_kInvalidHandle;
}

void HAL_CloseSerial(HAL_SerialPortHandle handle) {}

int HAL_GetSerialFD(HAL_SerialPortHandle handle, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

void HAL_SetSerialBaudRate(HAL_SerialPortHandle handle, int32_t baud,
                           int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetSerialDataBits(HAL_SerialPortHandle handle, int32_t bits,
                           int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetSerialParity(HAL_SerialPortHandle handle, int32_t parity,
                         int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetSerialStopBits(HAL_SerialPortHandle handle, int32_t stopBits,
                           int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetSerialWriteMode(HAL_SerialPortHandle handle, int32_t mode,
                            int32_t* status) {
  // This seems to be a no op on the NI serial port driver
}

void HAL_SetSerialFlowControl(HAL_SerialPortHandle handle, int32_t flow,
                              int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetSerialTimeout(HAL_SerialPortHandle handle, double timeout,
                          int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_EnableSerialTermination(HAL_SerialPortHandle handle, char terminator,
                                 int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_DisableSerialTermination(HAL_SerialPortHandle handle,
                                  int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetSerialReadBufferSize(HAL_SerialPortHandle handle, int32_t size,
                                 int32_t* status) {
  // NO OP currently
}

void HAL_SetSerialWriteBufferSize(HAL_SerialPortHandle handle, int32_t size,
                                  int32_t* status) {
  // NO OP currently
}

int32_t HAL_GetSerialBytesReceived(HAL_SerialPortHandle handle,
                                   int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return -1;
}

int32_t HAL_ReadSerial(HAL_SerialPortHandle handle, char* buffer, int32_t count,
                       int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return -1;
}

int32_t HAL_WriteSerial(HAL_SerialPortHandle handle, const char* buffer,
                        int32_t count, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return -1;
}

void HAL_FlushSerial(HAL_SerialPortHandle handle, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}
void HAL_ClearSerial(HAL_SerialPortHandle handle, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}
}  // extern "C"
