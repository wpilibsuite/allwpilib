/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/Interrupts.h"

#include <memory>

#include "ChipObject.h"

#include "DigitalInternal.h"
#include "HAL/Errors.h"
#include "HAL/cpp/make_unique.h"
#include "HAL/handles/HandlesInternal.h"
#include "HAL/handles/LimitedHandleResource.h"
#include "PortsInternal.h"

using namespace hal;

namespace {
struct Interrupt {
  std::unique_ptr<tInterrupt> anInterrupt;
  std::unique_ptr<tInterruptManager> manager;
};
}

static LimitedHandleResource<HAL_InterruptHandle, Interrupt, kNumInterrupts,
                             HAL_HandleEnum::Interrupt>
    interruptHandles;

extern "C" {

HAL_InterruptHandle HAL_InitializeInterrupts(HAL_Bool watcher,
                                             int32_t* status) {
  HAL_InterruptHandle handle = interruptHandles.Allocate();
  if (handle == HAL_kInvalidHandle) {
    *status = NO_AVAILABLE_RESOURCES;
    return HAL_kInvalidHandle;
  }
  auto anInterrupt = interruptHandles.Get(handle);
  uint32_t interruptIndex = static_cast<uint32_t>(getHandleIndex(handle));
  // Expects the calling leaf class to allocate an interrupt index.
  anInterrupt->anInterrupt.reset(tInterrupt::create(interruptIndex, status));
  anInterrupt->anInterrupt->writeConfig_WaitForAck(false, status);
  anInterrupt->manager = std::make_unique<tInterruptManager>(
      (1u << interruptIndex) | (1u << (interruptIndex + 8u)), watcher, status);
  return handle;
}

void HAL_CleanInterrupts(HAL_InterruptHandle interruptHandle, int32_t* status) {
  interruptHandles.Free(interruptHandle);
}

/**
 * In synchronous mode, wait for the defined interrupt to occur.
 * @param timeout Timeout in seconds
 * @param ignorePrevious If true, ignore interrupts that happened before
 * waitForInterrupt was called.
 * @return The mask of interrupts that fired.
 */
int64_t HAL_WaitForInterrupt(HAL_InterruptHandle interruptHandle,
                             double timeout, HAL_Bool ignorePrevious,
                             int32_t* status) {
  uint32_t result;
  auto anInterrupt = interruptHandles.Get(interruptHandle);
  if (anInterrupt == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  result = anInterrupt->manager->watch(static_cast<int32_t>(timeout * 1e3),
                                       ignorePrevious, status);

  // Don't report a timeout as an error - the return code is enough to tell
  // that a timeout happened.
  if (*status == -NiFpga_Status_IrqTimeout) {
    *status = NiFpga_Status_Success;
  }

  return result;
}

/**
 * Enable interrupts to occur on this input.
 * Interrupts are disabled when the RequestInterrupt call is made. This gives
 * time to do the setup of the other options before starting to field
 * interrupts.
 */
void HAL_EnableInterrupts(HAL_InterruptHandle interruptHandle,
                          int32_t* status) {
  auto anInterrupt = interruptHandles.Get(interruptHandle);
  if (anInterrupt == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  anInterrupt->manager->enable(status);
}

/**
 * Disable Interrupts without without deallocating structures.
 */
void HAL_DisableInterrupts(HAL_InterruptHandle interruptHandle,
                           int32_t* status) {
  auto anInterrupt = interruptHandles.Get(interruptHandle);
  if (anInterrupt == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  anInterrupt->manager->disable(status);
}

/**
 * Return the timestamp for the rising interrupt that occurred most recently.
 * This is in the same time domain as GetClock().
 * @return Timestamp in seconds since boot.
 */
double HAL_ReadInterruptRisingTimestamp(HAL_InterruptHandle interruptHandle,
                                        int32_t* status) {
  auto anInterrupt = interruptHandles.Get(interruptHandle);
  if (anInterrupt == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  uint32_t timestamp = anInterrupt->anInterrupt->readRisingTimeStamp(status);
  return timestamp * 1e-6;
}

/**
* Return the timestamp for the falling interrupt that occurred most recently.
* This is in the same time domain as GetClock().
* @return Timestamp in seconds since boot.
*/
double HAL_ReadInterruptFallingTimestamp(HAL_InterruptHandle interruptHandle,
                                         int32_t* status) {
  auto anInterrupt = interruptHandles.Get(interruptHandle);
  if (anInterrupt == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  uint32_t timestamp = anInterrupt->anInterrupt->readFallingTimeStamp(status);
  return timestamp * 1e-6;
}

void HAL_RequestInterrupts(HAL_InterruptHandle interruptHandle,
                           HAL_Handle digitalSourceHandle,
                           HAL_AnalogTriggerType analogTriggerType,
                           int32_t* status) {
  auto anInterrupt = interruptHandles.Get(interruptHandle);
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

void HAL_AttachInterruptHandler(HAL_InterruptHandle interruptHandle,
                                InterruptHandlerFunction handler, void* param,
                                int32_t* status) {
  auto anInterrupt = interruptHandles.Get(interruptHandle);
  if (anInterrupt == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  anInterrupt->manager->registerHandler(handler, param, status);
}

void HAL_SetInterruptUpSourceEdge(HAL_InterruptHandle interruptHandle,
                                  HAL_Bool risingEdge, HAL_Bool fallingEdge,
                                  int32_t* status) {
  auto anInterrupt = interruptHandles.Get(interruptHandle);
  if (anInterrupt == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  anInterrupt->anInterrupt->writeConfig_RisingEdge(risingEdge, status);
  anInterrupt->anInterrupt->writeConfig_FallingEdge(fallingEdge, status);
}

}  // extern "C"
