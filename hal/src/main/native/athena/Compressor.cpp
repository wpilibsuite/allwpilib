/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/Compressor.h"

#include "HALInitializer.h"
#include "PCMInternal.h"
#include "PortsInternal.h"
#include "ctre/PCM.h"
#include "hal/Errors.h"
#include "hal/handles/HandlesInternal.h"

using namespace hal;

namespace hal {
namespace init {
void InitializeCompressor() {}
}  // namespace init
}  // namespace hal

extern "C" {

HAL_CompressorHandle HAL_InitializeCompressor(int32_t module, int32_t* status) {
  hal::init::CheckInit();
  // Use status to check for invalid index
  initializePCM(module, status);
  if (*status != 0) {
    return HAL_kInvalidHandle;
  }

  // As compressors can have unlimited objects, just create a
  // handle with the module number as the index.

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
  bool value;

  *status = PCM_modules[index]->GetCompressor(value);

  return value;
}

void HAL_SetCompressorClosedLoopControl(HAL_CompressorHandle compressorHandle,
                                        HAL_Bool value, int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressorHandle, HAL_HandleEnum::Compressor, 0);
  if (index == InvalidHandleIndex) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  *status = PCM_modules[index]->SetClosedLoopControl(value);
}

HAL_Bool HAL_GetCompressorClosedLoopControl(
    HAL_CompressorHandle compressorHandle, int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressorHandle, HAL_HandleEnum::Compressor, 0);
  if (index == InvalidHandleIndex) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  bool value;

  *status = PCM_modules[index]->GetClosedLoopControl(value);

  return value;
}

HAL_Bool HAL_GetCompressorPressureSwitch(HAL_CompressorHandle compressorHandle,
                                         int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressorHandle, HAL_HandleEnum::Compressor, 0);
  if (index == InvalidHandleIndex) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  bool value;

  *status = PCM_modules[index]->GetPressure(value);

  return value;
}

double HAL_GetCompressorCurrent(HAL_CompressorHandle compressorHandle,
                                int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressorHandle, HAL_HandleEnum::Compressor, 0);
  if (index == InvalidHandleIndex) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  float value;

  *status = PCM_modules[index]->GetCompressorCurrent(value);

  return value;
}
HAL_Bool HAL_GetCompressorCurrentTooHighFault(
    HAL_CompressorHandle compressorHandle, int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressorHandle, HAL_HandleEnum::Compressor, 0);
  if (index == InvalidHandleIndex) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  bool value;

  *status = PCM_modules[index]->GetCompressorCurrentTooHighFault(value);

  return value;
}
HAL_Bool HAL_GetCompressorCurrentTooHighStickyFault(
    HAL_CompressorHandle compressorHandle, int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressorHandle, HAL_HandleEnum::Compressor, 0);
  if (index == InvalidHandleIndex) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  bool value;

  *status = PCM_modules[index]->GetCompressorCurrentTooHighStickyFault(value);

  return value;
}
HAL_Bool HAL_GetCompressorShortedStickyFault(
    HAL_CompressorHandle compressorHandle, int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressorHandle, HAL_HandleEnum::Compressor, 0);
  if (index == InvalidHandleIndex) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  bool value;

  *status = PCM_modules[index]->GetCompressorShortedStickyFault(value);

  return value;
}
HAL_Bool HAL_GetCompressorShortedFault(HAL_CompressorHandle compressorHandle,
                                       int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressorHandle, HAL_HandleEnum::Compressor, 0);
  if (index == InvalidHandleIndex) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  bool value;

  *status = PCM_modules[index]->GetCompressorShortedFault(value);

  return value;
}
HAL_Bool HAL_GetCompressorNotConnectedStickyFault(
    HAL_CompressorHandle compressorHandle, int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressorHandle, HAL_HandleEnum::Compressor, 0);
  if (index == InvalidHandleIndex) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  bool value;

  *status = PCM_modules[index]->GetCompressorNotConnectedStickyFault(value);

  return value;
}
HAL_Bool HAL_GetCompressorNotConnectedFault(
    HAL_CompressorHandle compressorHandle, int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressorHandle, HAL_HandleEnum::Compressor, 0);
  if (index == InvalidHandleIndex) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  bool value;

  *status = PCM_modules[index]->GetCompressorNotConnectedFault(value);

  return value;
}

}  // extern "C"
