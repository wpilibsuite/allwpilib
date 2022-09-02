// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/Interrupts.h"

#include <memory>

#include <wpi/SafeThread.h>

#include "DigitalInternal.h"
#include "HALInitializer.h"
#include "HALInternal.h"
#include "PortsInternal.h"
#include "hal/ChipObject.h"
#include "hal/Errors.h"
#include "hal/HALBase.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/handles/LimitedHandleResource.h"
#include "hal/roborio/InterruptManager.h"

using namespace hal;

namespace {

struct Interrupt {
  std::unique_ptr<tInterrupt> anInterrupt;
  InterruptManager& manager = InterruptManager::GetInstance();
  NiFpga_IrqContext irqContext = nullptr;
  uint32_t mask;
};

}  // namespace

static LimitedHandleResource<HAL_InterruptHandle, Interrupt, kNumInterrupts,
                             HAL_HandleEnum::Interrupt>* interruptHandles;

namespace hal::init {
void InitializeInterrupts() {
  static LimitedHandleResource<HAL_InterruptHandle, Interrupt, kNumInterrupts,
                               HAL_HandleEnum::Interrupt>
      iH;
  interruptHandles = &iH;
}
}  // namespace hal::init

extern "C" {

HAL_InterruptHandle HAL_InitializeInterrupts(int32_t* status) {
  hal::init::CheckInit();
  HAL_InterruptHandle handle = interruptHandles->Allocate();
  if (handle == HAL_kInvalidHandle) {
    *status = NO_AVAILABLE_RESOURCES;
    return HAL_kInvalidHandle;
  }
  auto anInterrupt = interruptHandles->Get(handle);
  uint32_t interruptIndex = static_cast<uint32_t>(getHandleIndex(handle));
  // Expects the calling leaf class to allocate an interrupt index.
  anInterrupt->anInterrupt.reset(tInterrupt::create(interruptIndex, status));
  anInterrupt->anInterrupt->writeConfig_WaitForAck(false, status);
  anInterrupt->irqContext = anInterrupt->manager.GetContext();
  anInterrupt->mask = (1u << interruptIndex) | (1u << (interruptIndex + 8u));
  return handle;
}

void HAL_CleanInterrupts(HAL_InterruptHandle interruptHandle) {
  auto anInterrupt = interruptHandles->Get(interruptHandle);
  interruptHandles->Free(interruptHandle);
  if (anInterrupt == nullptr) {
    return;
  }
  if (anInterrupt->irqContext) {
    anInterrupt->manager.ReleaseContext(anInterrupt->irqContext);
  }
}

int64_t HAL_WaitForInterrupt(HAL_InterruptHandle interruptHandle,
                             double timeout, HAL_Bool ignorePrevious,
                             int32_t* status) {
  uint32_t result;
  auto anInterrupt = interruptHandles->Get(interruptHandle);
  if (anInterrupt == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  result = anInterrupt->manager.WaitForInterrupt(
      anInterrupt->irqContext, anInterrupt->mask, ignorePrevious,
      static_cast<uint32_t>(timeout * 1e3), status);

  // Don't report a timeout as an error - the return code is enough to tell
  // that a timeout happened.
  if (*status == -NiFpga_Status_IrqTimeout) {
    *status = NiFpga_Status_Success;
  }

  return result;
}

int64_t HAL_WaitForMultipleInterrupts(HAL_InterruptHandle interruptHandle,
                                      int64_t mask, double timeout,
                                      HAL_Bool ignorePrevious,
                                      int32_t* status) {
  uint32_t result;
  auto anInterrupt = interruptHandles->Get(interruptHandle);
  if (anInterrupt == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  result = anInterrupt->manager.WaitForInterrupt(
      anInterrupt->irqContext, mask, ignorePrevious,
      static_cast<uint32_t>(timeout * 1e3), status);

  // Don't report a timeout as an error - the return code is enough to tell
  // that a timeout happened.
  if (*status == -NiFpga_Status_IrqTimeout) {
    *status = NiFpga_Status_Success;
  }

  return result;
}

int64_t HAL_ReadInterruptRisingTimestamp(HAL_InterruptHandle interruptHandle,
                                         int32_t* status) {
  auto anInterrupt = interruptHandles->Get(interruptHandle);
  if (anInterrupt == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  uint32_t timestamp = anInterrupt->anInterrupt->readRisingTimeStamp(status);
  return timestamp;
}

int64_t HAL_ReadInterruptFallingTimestamp(HAL_InterruptHandle interruptHandle,
                                          int32_t* status) {
  auto anInterrupt = interruptHandles->Get(interruptHandle);
  if (anInterrupt == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  uint32_t timestamp = anInterrupt->anInterrupt->readFallingTimeStamp(status);
  return timestamp;
}

void HAL_RequestInterrupts(HAL_InterruptHandle interruptHandle,
                           HAL_Handle digitalSourceHandle,
                           HAL_AnalogTriggerType analogTriggerType,
                           int32_t* status) {
  auto anInterrupt = interruptHandles->Get(interruptHandle);
  if (anInterrupt == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  anInterrupt->anInterrupt->writeConfig_WaitForAck(false, status);
  bool routingAnalogTrigger = false;
  uint8_t routingChannel = 0;
  uint8_t routingModule = 0;
  bool success =
      remapDigitalSource(digitalSourceHandle, analogTriggerType, routingChannel,
                         routingModule, routingAnalogTrigger);
  if (!success) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  anInterrupt->anInterrupt->writeConfig_Source_AnalogTrigger(
      routingAnalogTrigger, status);
  anInterrupt->anInterrupt->writeConfig_Source_Channel(routingChannel, status);
  anInterrupt->anInterrupt->writeConfig_Source_Module(routingModule, status);
}

void HAL_SetInterruptUpSourceEdge(HAL_InterruptHandle interruptHandle,
                                  HAL_Bool risingEdge, HAL_Bool fallingEdge,
                                  int32_t* status) {
  auto anInterrupt = interruptHandles->Get(interruptHandle);
  if (anInterrupt == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  anInterrupt->anInterrupt->writeConfig_RisingEdge(risingEdge, status);
  anInterrupt->anInterrupt->writeConfig_FallingEdge(fallingEdge, status);
}

void HAL_ReleaseWaitingInterrupt(HAL_InterruptHandle interruptHandle,
                                 int32_t* status) {
  auto anInterrupt = interruptHandles->Get(interruptHandle);
  if (anInterrupt == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  uint32_t interruptIndex =
      static_cast<uint32_t>(getHandleIndex(interruptHandle));

  hal::ReleaseFPGAInterrupt(interruptIndex);
  hal::ReleaseFPGAInterrupt(interruptIndex + 8);
}

}  // extern "C"
