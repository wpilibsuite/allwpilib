// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/CTREPCM.h"

#include <string>

#include "HALInitializer.hpp"
#include "HALInternal.hpp"
#include "PortsInternal.hpp"
#include "mockdata/CTREPCMDataInternal.hpp"
#include "wpi/hal/Errors.h"
#include "wpi/hal/handles/IndexedHandleResource.hpp"

using namespace wpi::hal;

namespace {
struct PCM {
  int32_t module;
  wpi::util::mutex lock;
  std::string previousAllocation;
};
}  // namespace

static IndexedHandleResource<HAL_CTREPCMHandle, PCM, kNumCTREPCMModules,
                             HAL_HandleEnum::CTREPCM>* pcmHandles;

namespace wpi::hal::init {
void InitializeCTREPCM() {
  static IndexedHandleResource<HAL_CTREPCMHandle, PCM, kNumCTREPCMModules,
                               HAL_HandleEnum::CTREPCM>
      pH;
  pcmHandles = &pH;
}
}  // namespace wpi::hal::init

HAL_CTREPCMHandle HAL_InitializeCTREPCM(int32_t busId, int32_t module,
                                        const char* allocationLocation,
                                        int32_t* status) {
  wpi::hal::init::CheckInit();

  HAL_CTREPCMHandle handle;
  auto pcm = pcmHandles->Allocate(module, &handle, status);

  if (*status != 0) {
    if (pcm) {
      wpi::hal::SetLastErrorPreviouslyAllocated(status, "CTRE PCM", module,
                                                pcm->previousAllocation);
    } else {
      wpi::hal::SetLastErrorIndexOutOfRange(status,
                                            "Invalid Index for CTRE PCM", 0,
                                            kNumCTREPCMModules - 1, module);
    }
    return HAL_kInvalidHandle;  // failed to allocate. Pass error back.
  }

  pcm->previousAllocation = allocationLocation ? allocationLocation : "";
  pcm->module = module;

  SimCTREPCMData[module].initialized = true;
  // Enable closed loop
  SimCTREPCMData[module].closedLoopEnabled = true;

  return handle;
}

void HAL_FreeCTREPCM(HAL_CTREPCMHandle handle) {
  auto pcm = pcmHandles->Get(handle);
  if (pcm == nullptr) {
    pcmHandles->Free(handle);
    return;
  }
  SimCTREPCMData[pcm->module].initialized = false;
  pcmHandles->Free(handle);
}

HAL_Bool HAL_CheckCTREPCMSolenoidChannel(int32_t channel) {
  return channel < kNumCTRESolenoidChannels && channel >= 0;
}

HAL_Bool HAL_GetCTREPCMCompressor(HAL_CTREPCMHandle handle, int32_t* status) {
  auto pcm = pcmHandles->Get(handle);
  if (pcm == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }

  return SimCTREPCMData[pcm->module].compressorOn;
}

void HAL_SetCTREPCMClosedLoopControl(HAL_CTREPCMHandle handle, HAL_Bool enabled,
                                     int32_t* status) {
  auto pcm = pcmHandles->Get(handle);
  if (pcm == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  SimCTREPCMData[pcm->module].closedLoopEnabled = enabled;
}

HAL_Bool HAL_GetCTREPCMClosedLoopControl(HAL_CTREPCMHandle handle,
                                         int32_t* status) {
  auto pcm = pcmHandles->Get(handle);
  if (pcm == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }

  return SimCTREPCMData[pcm->module].closedLoopEnabled;
}

HAL_Bool HAL_GetCTREPCMPressureSwitch(HAL_CTREPCMHandle handle,
                                      int32_t* status) {
  auto pcm = pcmHandles->Get(handle);
  if (pcm == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }

  return SimCTREPCMData[pcm->module].pressureSwitch;
}

double HAL_GetCTREPCMCompressorCurrent(HAL_CTREPCMHandle handle,
                                       int32_t* status) {
  auto pcm = pcmHandles->Get(handle);
  if (pcm == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return SimCTREPCMData[pcm->module].compressorCurrent;
}

HAL_Bool HAL_GetCTREPCMCompressorCurrentTooHighFault(HAL_CTREPCMHandle handle,
                                                     int32_t* status) {
  return false;
}

HAL_Bool HAL_GetCTREPCMCompressorCurrentTooHighStickyFault(
    HAL_CTREPCMHandle handle, int32_t* status) {
  return false;
}

HAL_Bool HAL_GetCTREPCMCompressorShortedStickyFault(HAL_CTREPCMHandle handle,
                                                    int32_t* status) {
  return false;
}

HAL_Bool HAL_GetCTREPCMCompressorShortedFault(HAL_CTREPCMHandle handle,
                                              int32_t* status) {
  return false;
}

HAL_Bool HAL_GetCTREPCMCompressorNotConnectedStickyFault(
    HAL_CTREPCMHandle handle, int32_t* status) {
  return false;
}

HAL_Bool HAL_GetCTREPCMCompressorNotConnectedFault(HAL_CTREPCMHandle handle,
                                                   int32_t* status) {
  return false;
}

int32_t HAL_GetCTREPCMSolenoids(HAL_CTREPCMHandle handle, int32_t* status) {
  auto pcm = pcmHandles->Get(handle);
  if (pcm == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  std::scoped_lock lock{pcm->lock};
  auto& data = SimCTREPCMData[pcm->module].solenoidOutput;
  uint8_t ret = 0;
  for (int i = 0; i < kNumCTRESolenoidChannels; i++) {
    ret |= (data[i] << i);
  }
  return ret;
}
void HAL_SetCTREPCMSolenoids(HAL_CTREPCMHandle handle, int32_t mask,
                             int32_t values, int32_t* status) {
  auto pcm = pcmHandles->Get(handle);
  if (pcm == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  auto& data = SimCTREPCMData[pcm->module].solenoidOutput;
  std::scoped_lock lock{pcm->lock};
  for (int i = 0; i < kNumCTRESolenoidChannels; i++) {
    auto indexMask = (1 << i);
    if ((mask & indexMask) != 0) {
      data[i] = (values & indexMask) != 0;
    }
  }
}

int32_t HAL_GetCTREPCMSolenoidDisabledList(HAL_CTREPCMHandle handle,
                                           int32_t* status) {
  return 0;
}

HAL_Bool HAL_GetCTREPCMSolenoidVoltageStickyFault(HAL_CTREPCMHandle handle,
                                                  int32_t* status) {
  return false;
}

HAL_Bool HAL_GetCTREPCMSolenoidVoltageFault(HAL_CTREPCMHandle handle,
                                            int32_t* status) {
  return false;
}

void HAL_ClearAllCTREPCMStickyFaults(HAL_CTREPCMHandle handle,
                                     int32_t* status) {}

void HAL_FireCTREPCMOneShot(HAL_CTREPCMHandle handle, int32_t index,
                            int32_t* status) {}
void HAL_SetCTREPCMOneShotDuration(HAL_CTREPCMHandle handle, int32_t index,
                                   int32_t durMs, int32_t* status) {}
