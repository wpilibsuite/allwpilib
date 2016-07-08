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

HAL_CompressorHandle HAL_InitializeCompressor(uint8_t module, int32_t* status) {
  // fail on invalid index;
  if (!HAL_CheckCompressorModule(module)) {
    *status = PARAMETER_OUT_OF_RANGE;
    return HAL_kInvalidHandle;
  }

  initializePCM(module);

  // As compressors can have unlimited objects, just create a
  // handle with the module number as the index.

  return (HAL_CompressorHandle)createHandle(module, HAL_HandleEnum::Compressor);
}

bool HAL_CheckCompressorModule(uint8_t module) {
  return module < kNumPCMModules;
}

bool HAL_GetCompressor(HAL_CompressorHandle compressor_handle,
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

void HAL_SetClosedLoopControl(HAL_CompressorHandle compressor_handle,
                              bool value, int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressor_handle, HAL_HandleEnum::Compressor);
  if (index == InvalidHandleIndex) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  PCM* module = PCM_modules[index];

  *status = module->SetClosedLoopControl(value);
}

bool HAL_GetClosedLoopControl(HAL_CompressorHandle compressor_handle,
                              int32_t* status) {
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

bool HAL_GetPressureSwitch(HAL_CompressorHandle compressor_handle,
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

float HAL_GetCompressorCurrent(HAL_CompressorHandle compressor_handle,
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
bool HAL_GetCompressorCurrentTooHighFault(
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
bool HAL_GetCompressorCurrentTooHighStickyFault(
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
bool HAL_GetCompressorShortedStickyFault(HAL_CompressorHandle compressor_handle,
                                         int32_t* status) {
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
bool HAL_GetCompressorShortedFault(HAL_CompressorHandle compressor_handle,
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
bool HAL_GetCompressorNotConnectedStickyFault(
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
bool HAL_GetCompressorNotConnectedFault(HAL_CompressorHandle compressor_handle,
                                        int32_t* status) {
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
void HAL_ClearAllPCMStickyFaults(HAL_CompressorHandle compressor_handle,
                                 int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressor_handle, HAL_HandleEnum::Compressor);
  if (index == InvalidHandleIndex) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  PCM* module = PCM_modules[index];

  *status = module->ClearStickyFaults();
}

}  // extern "C"
