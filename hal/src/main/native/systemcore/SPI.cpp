// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/SPI.h"

#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <array>
#include <atomic>
#include <cstdio>
#include <cstring>
#include <memory>

#include <fmt/format.h>
#include <wpi/mutex.h>
#include <wpi/print.h>

#include "HALInitializer.h"
#include "HALInternal.h"
#include "hal/DIO.h"
#include "hal/HAL.h"
#include "hal/handles/HandlesInternal.h"

using namespace hal;

namespace hal::init {
void InitializeSPI() {}
}  // namespace hal::init

extern "C" {

void HAL_InitializeSPI(HAL_SPIPort port, int32_t* status) {
  hal::init::CheckInit();
  *status = HAL_HANDLE_ERROR;
  return;
}

int32_t HAL_TransactionSPI(HAL_SPIPort port, const uint8_t* dataToSend,
                           uint8_t* dataReceived, int32_t size) {
  return -1;
}

int32_t HAL_WriteSPI(HAL_SPIPort port, const uint8_t* dataToSend,
                     int32_t sendSize) {
  return -1;
}

int32_t HAL_ReadSPI(HAL_SPIPort port, uint8_t* buffer, int32_t count) {
  return -1;
}

void HAL_CloseSPI(HAL_SPIPort port) {}

void HAL_SetSPISpeed(HAL_SPIPort port, int32_t speed) {}

void HAL_SetSPIMode(HAL_SPIPort port, HAL_SPIMode mode) {}

HAL_SPIMode HAL_GetSPIMode(HAL_SPIPort port) {
  return HAL_SPI_kMode0;
}

void HAL_SetSPIChipSelectActiveHigh(HAL_SPIPort port, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetSPIChipSelectActiveLow(HAL_SPIPort port, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

int32_t HAL_GetSPIHandle(HAL_SPIPort port) {
  return 0;
}

void HAL_SetSPIHandle(HAL_SPIPort port, int32_t handle) {}

void HAL_InitSPIAuto(HAL_SPIPort port, int32_t bufferSize, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_FreeSPIAuto(HAL_SPIPort port, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_StartSPIAutoRate(HAL_SPIPort port, double period, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_StartSPIAutoTrigger(HAL_SPIPort port, HAL_Handle digitalSourceHandle,
                             HAL_AnalogTriggerType analogTriggerType,
                             HAL_Bool triggerRising, HAL_Bool triggerFalling,
                             int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_StopSPIAuto(HAL_SPIPort port, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetSPIAutoTransmitData(HAL_SPIPort port, const uint8_t* dataToSend,
                                int32_t dataSize, int32_t zeroSize,
                                int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_ForceSPIAutoRead(HAL_SPIPort port, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

int32_t HAL_ReadSPIAutoReceivedData(HAL_SPIPort port, uint32_t* buffer,
                                    int32_t numToRead, double timeout,
                                    int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

int32_t HAL_GetSPIAutoDroppedCount(HAL_SPIPort port, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

void HAL_ConfigureSPIAutoStall(HAL_SPIPort port, int32_t csToSclkTicks,
                               int32_t stallTicks, int32_t pow2BytesPerRead,
                               int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

}  // extern "C"
