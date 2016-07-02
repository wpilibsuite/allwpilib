/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/Compressor.h"

#include "HAL/Errors.h"
#include "ctre/PCM.h"
#include "handles/HandlesInternal.h"

static const int NUM_MODULE_NUMBERS = 63;
extern PCM* PCM_modules[NUM_MODULE_NUMBERS];
extern void initializePCM(int module);

using namespace hal;

extern "C" {

HalCompressorHandle initializeCompressor(uint8_t module, int32_t* status) {
  // fail on invalid index;
  if (!checkCompressorModule(module)) {
    *status = PARAMETER_OUT_OF_RANGE;
    return HAL_INVALID_HANDLE;
  }

  initializePCM(module);

  // As compressors can have unlimited objects, just create a
  // handle with the module number as the index.

  return (HalCompressorHandle)createHandle(module, HalHandleEnum::Compressor);
}

bool checkCompressorModule(uint8_t module) {
  return module < NUM_MODULE_NUMBERS;
}

bool getCompressor(HalCompressorHandle compressor_handle, int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressor_handle, HalHandleEnum::Compressor);
  if (index == InvalidHandleIndex) {
    *status = PARAMETER_OUT_OF_RANGE;
    return false;
  }
  PCM* module = PCM_modules[index];
  bool value;

  *status = module->GetCompressor(value);

  return value;
}

void setClosedLoopControl(HalCompressorHandle compressor_handle, bool value,
                          int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressor_handle, HalHandleEnum::Compressor);
  if (index == InvalidHandleIndex) {
    *status = PARAMETER_OUT_OF_RANGE;
    return;
  }
  PCM* module = PCM_modules[index];

  *status = module->SetClosedLoopControl(value);
}

bool getClosedLoopControl(HalCompressorHandle compressor_handle,
                          int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressor_handle, HalHandleEnum::Compressor);
  if (index == InvalidHandleIndex) {
    *status = PARAMETER_OUT_OF_RANGE;
    return false;
  }
  PCM* module = PCM_modules[index];
  bool value;

  *status = module->GetClosedLoopControl(value);

  return value;
}

bool getPressureSwitch(HalCompressorHandle compressor_handle, int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressor_handle, HalHandleEnum::Compressor);
  if (index == InvalidHandleIndex) {
    *status = PARAMETER_OUT_OF_RANGE;
    return false;
  }
  PCM* module = PCM_modules[index];
  bool value;

  *status = module->GetPressure(value);

  return value;
}

float getCompressorCurrent(HalCompressorHandle compressor_handle,
                           int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressor_handle, HalHandleEnum::Compressor);
  if (index == InvalidHandleIndex) {
    *status = PARAMETER_OUT_OF_RANGE;
    return 0;
  }
  PCM* module = PCM_modules[index];
  float value;

  *status = module->GetCompressorCurrent(value);

  return value;
}
bool getCompressorCurrentTooHighFault(HalCompressorHandle compressor_handle,
                                      int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressor_handle, HalHandleEnum::Compressor);
  if (index == InvalidHandleIndex) {
    *status = PARAMETER_OUT_OF_RANGE;
    return false;
  }
  PCM* module = PCM_modules[index];
  bool value;

  *status = module->GetCompressorCurrentTooHighFault(value);

  return value;
}
bool getCompressorCurrentTooHighStickyFault(
    HalCompressorHandle compressor_handle, int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressor_handle, HalHandleEnum::Compressor);
  if (index == InvalidHandleIndex) {
    *status = PARAMETER_OUT_OF_RANGE;
    return false;
  }
  PCM* module = PCM_modules[index];
  bool value;

  *status = module->GetCompressorCurrentTooHighStickyFault(value);

  return value;
}
bool getCompressorShortedStickyFault(HalCompressorHandle compressor_handle,
                                     int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressor_handle, HalHandleEnum::Compressor);
  if (index == InvalidHandleIndex) {
    *status = PARAMETER_OUT_OF_RANGE;
    return false;
  }
  PCM* module = PCM_modules[index];
  bool value;

  *status = module->GetCompressorShortedStickyFault(value);

  return value;
}
bool getCompressorShortedFault(HalCompressorHandle compressor_handle,
                               int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressor_handle, HalHandleEnum::Compressor);
  if (index == InvalidHandleIndex) {
    *status = PARAMETER_OUT_OF_RANGE;
    return false;
  }
  PCM* module = PCM_modules[index];
  bool value;

  *status = module->GetCompressorShortedFault(value);

  return value;
}
bool getCompressorNotConnectedStickyFault(HalCompressorHandle compressor_handle,
                                          int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressor_handle, HalHandleEnum::Compressor);
  if (index == InvalidHandleIndex) {
    *status = PARAMETER_OUT_OF_RANGE;
    return false;
  }
  PCM* module = PCM_modules[index];
  bool value;

  *status = module->GetCompressorNotConnectedStickyFault(value);

  return value;
}
bool getCompressorNotConnectedFault(HalCompressorHandle compressor_handle,
                                    int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressor_handle, HalHandleEnum::Compressor);
  if (index == InvalidHandleIndex) {
    *status = PARAMETER_OUT_OF_RANGE;
    return false;
  }
  PCM* module = PCM_modules[index];
  bool value;

  *status = module->GetCompressorNotConnectedFault(value);

  return value;
}
void clearAllPCMStickyFaults(HalCompressorHandle compressor_handle,
                             int32_t* status) {
  int16_t index =
      getHandleTypedIndex(compressor_handle, HalHandleEnum::Compressor);
  if (index == InvalidHandleIndex) {
    *status = PARAMETER_OUT_OF_RANGE;
    return;
  }
  PCM* module = PCM_modules[index];

  *status = module->ClearStickyFaults();
}

}  // extern "C"
