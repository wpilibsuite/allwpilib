/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/Compressor.h"

#include "HAL/Errors.h"
#include "PCMInternal.h"
#include "PortsInternal.h"
#include "ctre/PCM.h"
#include "handles/HandlesInternal.h"

using namespace hal;

extern "C" {

HAL_CompressorHandle HAL_InitializeCompressor(int32_t module, int32_t* status) {
  // Use status to check for invalid index
  initializePCM(module, status);
  if (*status != 0) {
    return HAL_kInvalidHandle;
  }

  // As compressors can have unlimited objects, just create a
  // handle with the module number as the index.

  return (HAL_CompressorHandle)createHandle(static_cast<int16_t>(module),
                                            HAL_HandleEnum::Compressor);
}

HAL_Bool HAL_CheckCompressorModule(int32_t module) {
  return (module < kNumPCMModules) && (module >= 0);
}

HAL_Bool HAL_GetCompressor(HAL_CompressorHandle compressor_handle,
                           int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressor_handle, HAL_HandleEnum::Compressor);
  if (index == InvalidHandleIndex) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  PCM* module = PCM_modules[index];
  bool value;

  *status = module->GetCompressor(value);

  return value;
}

void HAL_SetCompressorClosedLoopControl(HAL_CompressorHandle compressor_handle,
                                        HAL_Bool value, int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressor_handle, HAL_HandleEnum::Compressor);
  if (index == InvalidHandleIndex) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  PCM* module = PCM_modules[index];

  *status = module->SetClosedLoopControl(value);
}

HAL_Bool HAL_GetCompressorClosedLoopControl(
    HAL_CompressorHandle compressor_handle, int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressor_handle, HAL_HandleEnum::Compressor);
  if (index == InvalidHandleIndex) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  PCM* module = PCM_modules[index];
  bool value;

  *status = module->GetClosedLoopControl(value);

  return value;
}

HAL_Bool HAL_GetCompressorPressureSwitch(HAL_CompressorHandle compressor_handle,
                                         int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressor_handle, HAL_HandleEnum::Compressor);
  if (index == InvalidHandleIndex) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  PCM* module = PCM_modules[index];
  bool value;

  *status = module->GetPressure(value);

  return value;
}

double HAL_GetCompressorCurrent(HAL_CompressorHandle compressor_handle,
                                int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressor_handle, HAL_HandleEnum::Compressor);
  if (index == InvalidHandleIndex) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  PCM* module = PCM_modules[index];
  float value;

  *status = module->GetCompressorCurrent(value);

  return value;
}
HAL_Bool HAL_GetCompressorCurrentTooHighFault(
    HAL_CompressorHandle compressor_handle, int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressor_handle, HAL_HandleEnum::Compressor);
  if (index == InvalidHandleIndex) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  PCM* module = PCM_modules[index];
  bool value;

  *status = module->GetCompressorCurrentTooHighFault(value);

  return value;
}
HAL_Bool HAL_GetCompressorCurrentTooHighStickyFault(
    HAL_CompressorHandle compressor_handle, int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressor_handle, HAL_HandleEnum::Compressor);
  if (index == InvalidHandleIndex) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  PCM* module = PCM_modules[index];
  bool value;

  *status = module->GetCompressorCurrentTooHighStickyFault(value);

  return value;
}
HAL_Bool HAL_GetCompressorShortedStickyFault(
    HAL_CompressorHandle compressor_handle, int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressor_handle, HAL_HandleEnum::Compressor);
  if (index == InvalidHandleIndex) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  PCM* module = PCM_modules[index];
  bool value;

  *status = module->GetCompressorShortedStickyFault(value);

  return value;
}
HAL_Bool HAL_GetCompressorShortedFault(HAL_CompressorHandle compressor_handle,
                                       int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressor_handle, HAL_HandleEnum::Compressor);
  if (index == InvalidHandleIndex) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  PCM* module = PCM_modules[index];
  bool value;

  *status = module->GetCompressorShortedFault(value);

  return value;
}
HAL_Bool HAL_GetCompressorNotConnectedStickyFault(
    HAL_CompressorHandle compressor_handle, int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressor_handle, HAL_HandleEnum::Compressor);
  if (index == InvalidHandleIndex) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  PCM* module = PCM_modules[index];
  bool value;

  *status = module->GetCompressorNotConnectedStickyFault(value);

  return value;
}
HAL_Bool HAL_GetCompressorNotConnectedFault(
    HAL_CompressorHandle compressor_handle, int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressor_handle, HAL_HandleEnum::Compressor);
  if (index == InvalidHandleIndex) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  PCM* module = PCM_modules[index];
  bool value;

  *status = module->GetCompressorNotConnectedFault(value);

  return value;
}
}  // extern "C"
