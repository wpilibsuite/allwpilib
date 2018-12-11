/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/SPI.h"

#include "HALInitializer.h"
#include "mockdata/SPIDataInternal.h"

using namespace hal;

namespace hal {
namespace init {
void InitializeSPI() {}
}  // namespace init
}  // namespace hal

void HAL_InitializeSPI(HAL_SPIPort port, int32_t* status) {
  hal::init::CheckInit();
  SimSPIData[port].initialized = true;
}
int32_t HAL_TransactionSPI(HAL_SPIPort port, const uint8_t* dataToSend,
                           uint8_t* dataReceived, int32_t size) {
  return SimSPIData[port].Transaction(dataToSend, dataReceived, size);
}
int32_t HAL_WriteSPI(HAL_SPIPort port, const uint8_t* dataToSend,
                     int32_t sendSize) {
  return SimSPIData[port].Write(dataToSend, sendSize);
}
int32_t HAL_ReadSPI(HAL_SPIPort port, uint8_t* buffer, int32_t count) {
  return SimSPIData[port].Read(buffer, count);
}
void HAL_CloseSPI(HAL_SPIPort port) { SimSPIData[port].initialized = false; }
void HAL_SetSPISpeed(HAL_SPIPort port, int32_t speed) {}
void HAL_SetSPIOpts(HAL_SPIPort port, HAL_Bool msbFirst,
                    HAL_Bool sampleOnTrailing, HAL_Bool clkIdleHigh) {}
void HAL_SetSPIChipSelectActiveHigh(HAL_SPIPort port, int32_t* status) {}
void HAL_SetSPIChipSelectActiveLow(HAL_SPIPort port, int32_t* status) {}
int32_t HAL_GetSPIHandle(HAL_SPIPort port) { return 0; }
void HAL_SetSPIHandle(HAL_SPIPort port, int32_t handle) {}

void HAL_InitSPIAuto(HAL_SPIPort port, int32_t bufferSize, int32_t* status) {}
void HAL_FreeSPIAuto(HAL_SPIPort port, int32_t* status) {}
void HAL_StartSPIAutoRate(HAL_SPIPort port, double period, int32_t* status) {}
void HAL_StartSPIAutoTrigger(HAL_SPIPort port, HAL_Handle digitalSourceHandle,
                             HAL_AnalogTriggerType analogTriggerType,
                             HAL_Bool triggerRising, HAL_Bool triggerFalling,
                             int32_t* status) {}
void HAL_StopSPIAuto(HAL_SPIPort port, int32_t* status) {}
void HAL_SetSPIAutoTransmitData(HAL_SPIPort port, const uint8_t* dataToSend,
                                int32_t dataSize, int32_t zeroSize,
                                int32_t* status) {}
void HAL_ForceSPIAutoRead(HAL_SPIPort port, int32_t* status) {}
int32_t HAL_ReadSPIAutoReceivedData(HAL_SPIPort port, uint32_t* buffer,
                                    int32_t numToRead, double timeout,
                                    int32_t* status) {
  return SimSPIData[port].ReadAutoReceivedData(buffer, numToRead, timeout,
                                               status);
}
int32_t HAL_GetSPIAutoDroppedCount(HAL_SPIPort port, int32_t* status) {
  return 0;
}
