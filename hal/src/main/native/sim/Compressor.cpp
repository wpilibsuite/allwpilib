/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/Compressor.h"

#include "HAL/Errors.h"
#include "HAL/handles/HandlesInternal.h"
#include "MockData/PCMDataInternal.h"
#include "PortsInternal.h"

using namespace hal;

namespace hal {
namespace init {
void InitializeCompressor() {}
}  // namespace init
}  // namespace hal

extern "C" {

HAL_CompressorHandle HAL_InitializeCompressor(int32_t module, int32_t* status) {
  // As compressors can have unlimited objects, just create a
  // handle with the module number as the index.

  SimPCMData[module].SetCompressorInitialized(true);
  return (HAL_CompressorHandle)createHandle(static_cast<int16_t>(module),
                                            HAL_HandleEnum::Compressor, 0);
}

HAL_Bool HAL_CheckCompressorModule(int32_t module) {
  return module < kNumPCMModules && module >= 0;
}

HAL_Bool HAL_GetCompressor(HAL_CompressorHandle compressorHandle,
                           int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressorHandle, HAL_HandleEnum::Compressor, 0);
  if (index == InvalidHandleIndex) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }

  return SimPCMData[index].GetCompressorOn();
}

void HAL_SetCompressorClosedLoopControl(HAL_CompressorHandle compressorHandle,
                                        HAL_Bool value, int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressorHandle, HAL_HandleEnum::Compressor, 0);
  if (index == InvalidHandleIndex) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  SimPCMData[index].SetClosedLoopEnabled(value);
}

HAL_Bool HAL_GetCompressorClosedLoopControl(
    HAL_CompressorHandle compressorHandle, int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressorHandle, HAL_HandleEnum::Compressor, 0);
  if (index == InvalidHandleIndex) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }

  return SimPCMData[index].GetClosedLoopEnabled();
}

HAL_Bool HAL_GetCompressorPressureSwitch(HAL_CompressorHandle compressorHandle,
                                         int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressorHandle, HAL_HandleEnum::Compressor, 0);
  if (index == InvalidHandleIndex) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }

  return SimPCMData[index].GetPressureSwitch();
}

double HAL_GetCompressorCurrent(HAL_CompressorHandle compressorHandle,
                                int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressorHandle, HAL_HandleEnum::Compressor, 0);
  if (index == InvalidHandleIndex) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return SimPCMData[index].GetCompressorCurrent();
}
HAL_Bool HAL_GetCompressorCurrentTooHighFault(
    HAL_CompressorHandle compressorHandle, int32_t* status) {
  return false;
}
HAL_Bool HAL_GetCompressorCurrentTooHighStickyFault(
    HAL_CompressorHandle compressorHandle, int32_t* status) {
  return false;
}
HAL_Bool HAL_GetCompressorShortedStickyFault(
    HAL_CompressorHandle compressorHandle, int32_t* status) {
  return false;
}
HAL_Bool HAL_GetCompressorShortedFault(HAL_CompressorHandle compressorHandle,
                                       int32_t* status) {
  return false;
}
HAL_Bool HAL_GetCompressorNotConnectedStickyFault(
    HAL_CompressorHandle compressorHandle, int32_t* status) {
  return false;
}
HAL_Bool HAL_GetCompressorNotConnectedFault(
    HAL_CompressorHandle compressorHandle, int32_t* status) {
  return false;
}
}  // extern "C"
