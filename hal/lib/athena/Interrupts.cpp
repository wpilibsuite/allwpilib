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
#include "PortsInternal.h"
#include "handles/HandlesInternal.h"
#include "handles/LimitedHandleResource.h"

using namespace hal;

namespace {
struct Interrupt  // FIXME: why is this internal?
{
  tInterrupt* anInterrupt;
  tInterruptManager* manager;
};
}

static LimitedHandleResource<HalInterruptHandle, Interrupt, kNumInterrupts,
                             HalHandleEnum::Interrupt>
    interruptHandles;

extern "C" {

HalInterruptHandle initializeInterrupts(bool watcher, int32_t* status) {
  HalInterruptHandle handle = interruptHandles.Allocate();
  if (handle == HAL_INVALID_HANDLE) {
    *status = NO_AVAILABLE_RESOURCES;
    return HAL_INVALID_HANDLE;
  }
  auto anInterrupt = interruptHandles.Get(handle);
  uint32_t interruptIndex = static_cast<uint32_t>(getHandleIndex(handle));
  // Expects the calling leaf class to allocate an interrupt index.
  anInterrupt->anInterrupt = tInterrupt::create(interruptIndex, status);
  anInterrupt->anInterrupt->writeConfig_WaitForAck(false, status);
  anInterrupt->manager = new tInterruptManager(
      (1 << interruptIndex) | (1 << (interruptIndex + 8)), watcher, status);
  return handle;
}

void cleanInterrupts(HalInterruptHandle interrupt_handle, int32_t* status) {
  auto anInterrupt = interruptHandles.Get(interrupt_handle);
  if (anInterrupt == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  interruptHandles.Free(interrupt_handle);
  delete anInterrupt->anInterrupt;
  delete anInterrupt->manager;
}

/**
 * In synchronous mode, wait for the defined interrupt to occur.
 * @param timeout Timeout in seconds
 * @param ignorePrevious If true, ignore interrupts that happened before
 * waitForInterrupt was called.
 * @return The mask of interrupts that fired.
 */
uint32_t waitForInterrupt(HalInterruptHandle interrupt_handle, double timeout,
                          bool ignorePrevious, int32_t* status) {
  uint32_t result;
  auto anInterrupt = interruptHandles.Get(interrupt_handle);
  if (anInterrupt == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  result = anInterrupt->manager->watch((int32_t)(timeout * 1e3), ignorePrevious,
                                       status);

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
void enableInterrupts(HalInterruptHandle interrupt_handle, int32_t* status) {
  auto anInterrupt = interruptHandles.Get(interrupt_handle);
  if (anInterrupt == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  anInterrupt->manager->enable(status);
}

/**
 * Disable Interrupts without without deallocating structures.
 */
void disableInterrupts(HalInterruptHandle interrupt_handle, int32_t* status) {
  auto anInterrupt = interruptHandles.Get(interrupt_handle);
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
double readRisingTimestamp(HalInterruptHandle interrupt_handle,
                           int32_t* status) {
  auto anInterrupt = interruptHandles.Get(interrupt_handle);
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
double readFallingTimestamp(HalInterruptHandle interrupt_handle,
                            int32_t* status) {
  auto anInterrupt = interruptHandles.Get(interrupt_handle);
  if (anInterrupt == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  uint32_t timestamp = anInterrupt->anInterrupt->readFallingTimeStamp(status);
  return timestamp * 1e-6;
}

void requestInterrupts(HalInterruptHandle interrupt_handle,
                       HalHandle digitalSourceHandle,
                       AnalogTriggerType analogTriggerType, int32_t* status) {
  auto anInterrupt = interruptHandles.Get(interrupt_handle);
  if (anInterrupt == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  anInterrupt->anInterrupt->writeConfig_WaitForAck(false, status);
  bool routingAnalogTrigger = false;
  uint32_t routingPin = 0;
  uint8_t routingModule = 0;
  bool success =
      remapDigitalSource(digitalSourceHandle, analogTriggerType, routingPin,
                         routingModule, routingAnalogTrigger);
  if (!success) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  anInterrupt->anInterrupt->writeConfig_Source_AnalogTrigger(
      routingAnalogTrigger, status);
  anInterrupt->anInterrupt->writeConfig_Source_Channel(routingPin, status);
  anInterrupt->anInterrupt->writeConfig_Source_Module(routingModule, status);
}

void attachInterruptHandler(HalInterruptHandle interrupt_handle,
                            InterruptHandlerFunction handler, void* param,
                            int32_t* status) {
  auto anInterrupt = interruptHandles.Get(interrupt_handle);
  if (anInterrupt == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  anInterrupt->manager->registerHandler(handler, param, status);
}

void setInterruptUpSourceEdge(HalInterruptHandle interrupt_handle,
                              bool risingEdge, bool fallingEdge,
                              int32_t* status) {
  auto anInterrupt = interruptHandles.Get(interrupt_handle);
  if (anInterrupt == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  anInterrupt->anInterrupt->writeConfig_RisingEdge(risingEdge, status);
  anInterrupt->anInterrupt->writeConfig_FallingEdge(fallingEdge, status);
}

}  // extern "C"
