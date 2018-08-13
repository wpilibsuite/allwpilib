/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/SerialPort.h"

#include "HALInitializer.h"

namespace hal {
namespace init {
void InitializeSerialPort() {}
}  // namespace init
}  // namespace hal

extern "C" {
void HAL_InitializeSerialPort(HAL_SerialPort port, int32_t* status) {
  hal::init::CheckInit();
}

void HAL_InitializeSerialPortDirect(HAL_SerialPort port, const char* portName,
                                    int32_t* status) {}

void HAL_SetSerialBaudRate(HAL_SerialPort port, int32_t baud, int32_t* status) {
}

void HAL_SetSerialDataBits(HAL_SerialPort port, int32_t bits, int32_t* status) {
}

void HAL_SetSerialParity(HAL_SerialPort port, int32_t parity, int32_t* status) {
}

void HAL_SetSerialStopBits(HAL_SerialPort port, int32_t stopBits,
                           int32_t* status) {}

void HAL_SetSerialWriteMode(HAL_SerialPort port, int32_t mode,
                            int32_t* status) {}

void HAL_SetSerialFlowControl(HAL_SerialPort port, int32_t flow,
                              int32_t* status) {}

void HAL_SetSerialTimeout(HAL_SerialPort port, double timeout,
                          int32_t* status) {}

void HAL_EnableSerialTermination(HAL_SerialPort port, char terminator,
                                 int32_t* status) {}

void HAL_DisableSerialTermination(HAL_SerialPort port, int32_t* status) {}

void HAL_SetSerialReadBufferSize(HAL_SerialPort port, int32_t size,
                                 int32_t* status) {}

void HAL_SetSerialWriteBufferSize(HAL_SerialPort port, int32_t size,
                                  int32_t* status) {}

int32_t HAL_GetSerialBytesReceived(HAL_SerialPort port, int32_t* status) {
  return 0;
}

int32_t HAL_ReadSerial(HAL_SerialPort port, char* buffer, int32_t count,
                       int32_t* status) {
  return 0;
}

int32_t HAL_WriteSerial(HAL_SerialPort port, const char* buffer, int32_t count,
                        int32_t* status) {
  return 0;
}

void HAL_FlushSerial(HAL_SerialPort port, int32_t* status) {}

void HAL_ClearSerial(HAL_SerialPort port, int32_t* status) {}

void HAL_CloseSerial(HAL_SerialPort port, int32_t* status) {}
}  // extern "C"
