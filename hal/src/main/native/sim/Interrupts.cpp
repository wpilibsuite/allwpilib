// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/Interrupts.h"

#include <memory>

#include <wpi/condition_variable.h>

#include "AnalogInternal.h"
#include "DigitalInternal.h"
#include "ErrorsInternal.h"
#include "HALInitializer.h"
#include "MockHooksInternal.h"
#include "PortsInternal.h"
#include "hal/AnalogTrigger.h"
#include "hal/Errors.h"
#include "hal/Value.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/handles/LimitedHandleResource.h"
#include "hal/handles/UnlimitedHandleResource.h"
#include "mockdata/AnalogInDataInternal.h"
#include "mockdata/DIODataInternal.h"

#ifdef _WIN32
#pragma warning(disable : 4996 4018 6297 26451 4334)
#endif

using namespace hal;

enum WaitResult {
  Timeout = 0x0,
  RisingEdge = 0x1,
  FallingEdge = 0x100,
  Both = 0x101,
};

namespace {
struct Interrupt {
  bool isAnalog;
  HAL_Handle portHandle;
  uint8_t index;
  HAL_AnalogTriggerType trigType;
  int64_t risingTimestamp;
  int64_t fallingTimestamp;
  bool currentState;
  bool fireOnUp;
  bool fireOnDown;
  int32_t callbackId;
};

struct SynchronousWaitData {
  HAL_InterruptHandle interruptHandle{HAL_kInvalidHandle};
  wpi::condition_variable waitCond;
  HAL_Bool waitPredicate{false};
};
}  // namespace

static LimitedHandleResource<HAL_InterruptHandle, Interrupt, kNumInterrupts,
                             HAL_HandleEnum::Interrupt>* interruptHandles;

using SynchronousWaitDataHandle = HAL_Handle;
static UnlimitedHandleResource<SynchronousWaitDataHandle, SynchronousWaitData,
                               HAL_HandleEnum::Vendor>*
    synchronousInterruptHandles;

namespace hal::init {
void InitializeInterrupts() {
  static LimitedHandleResource<HAL_InterruptHandle, Interrupt, kNumInterrupts,
                               HAL_HandleEnum::Interrupt>
      iH;
  interruptHandles = &iH;
  static UnlimitedHandleResource<SynchronousWaitDataHandle, SynchronousWaitData,
                                 HAL_HandleEnum::Vendor>
      siH;
  synchronousInterruptHandles = &siH;
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
  if (anInterrupt == nullptr) {  // would only occur on thread issue.
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }

  anInterrupt->index = getHandleIndex(handle);
  anInterrupt->callbackId = -1;

  return handle;
}
void HAL_CleanInterrupts(HAL_InterruptHandle interruptHandle) {
  interruptHandles->Free(interruptHandle);
}

static void ProcessInterruptDigitalSynchronous(const char* name, void* param,
                                               const struct HAL_Value* value) {
  // void* is a SynchronousWaitDataHandle.
  // convert to uintptr_t first, then to handle
  uintptr_t handleTmp = reinterpret_cast<uintptr_t>(param);
  SynchronousWaitDataHandle handle =
      static_cast<SynchronousWaitDataHandle>(handleTmp);
  auto interruptData = synchronousInterruptHandles->Get(handle);
  if (interruptData == nullptr) {
    return;
  }
  auto interrupt = interruptHandles->Get(interruptData->interruptHandle);
  if (interrupt == nullptr) {
    return;
  }
  // Have a valid interrupt
  if (value->type != HAL_Type::HAL_BOOLEAN) {
    return;
  }
  bool retVal = value->data.v_boolean;
  auto previousState = interrupt->currentState;
  interrupt->currentState = retVal;
  // If no change in interrupt, return;
  if (retVal == previousState) {
    return;
  }
  // If its a falling change, and we dont fire on falling return
  if (previousState && !interrupt->fireOnDown) {
    return;
  }
  // If its a rising change, and we dont fire on rising return.
  if (!previousState && !interrupt->fireOnUp) {
    return;
  }

  interruptData->waitPredicate = true;

  // Pulse interrupt
  interruptData->waitCond.notify_all();
}

static double GetAnalogTriggerValue(HAL_Handle triggerHandle,
                                    HAL_AnalogTriggerType type,
                                    int32_t* status) {
  return HAL_GetAnalogTriggerOutput(triggerHandle, type, status);
}

static void ProcessInterruptAnalogSynchronous(const char* name, void* param,
                                              const struct HAL_Value* value) {
  // void* is a SynchronousWaitDataHandle.
  // convert to uintptr_t first, then to handle
  uintptr_t handleTmp = reinterpret_cast<uintptr_t>(param);
  SynchronousWaitDataHandle handle =
      static_cast<SynchronousWaitDataHandle>(handleTmp);
  auto interruptData = synchronousInterruptHandles->Get(handle);
  if (interruptData == nullptr) {
    return;
  }
  auto interrupt = interruptHandles->Get(interruptData->interruptHandle);
  if (interrupt == nullptr) {
    return;
  }
  // Have a valid interrupt
  if (value->type != HAL_Type::HAL_DOUBLE) {
    return;
  }
  int32_t status = 0;
  bool retVal = GetAnalogTriggerValue(interrupt->portHandle,
                                      interrupt->trigType, &status);
  if (status != 0) {
    // Interrupt and Cancel
    interruptData->waitPredicate = true;
    // Pulse interrupt
    interruptData->waitCond.notify_all();
  }
  auto previousState = interrupt->currentState;
  interrupt->currentState = retVal;
  // If no change in interrupt, return;
  if (retVal == previousState) {
    return;
  }
  // If its a falling change, and we dont fire on falling return
  if (previousState && !interrupt->fireOnDown) {
    return;
  }
  // If its a rising change, and we dont fire on rising return.
  if (!previousState && !interrupt->fireOnUp) {
    return;
  }

  interruptData->waitPredicate = true;

  // Pulse interrupt
  interruptData->waitCond.notify_all();
}

static int64_t WaitForInterruptDigital(HAL_InterruptHandle handle,
                                       Interrupt* interrupt, double timeout,
                                       bool ignorePrevious) {
  auto data = std::make_shared<SynchronousWaitData>();

  auto dataHandle = synchronousInterruptHandles->Allocate(data);
  if (dataHandle == HAL_kInvalidHandle) {
    // Error allocating data
    return WaitResult::Timeout;
  }

  // auto data = synchronousInterruptHandles->Get(dataHandle);
  data->waitPredicate = false;
  data->interruptHandle = handle;

  int32_t status = 0;

  int32_t digitalIndex = GetDigitalInputChannel(interrupt->portHandle, &status);

  if (status != 0) {
    return WaitResult::Timeout;
  }

  interrupt->currentState = SimDIOData[digitalIndex].value;

  int32_t uid = SimDIOData[digitalIndex].value.RegisterCallback(
      &ProcessInterruptDigitalSynchronous,
      reinterpret_cast<void*>(static_cast<uintptr_t>(dataHandle)), false);

  bool timedOut = false;

  wpi::mutex waitMutex;

  auto timeoutTime =
      std::chrono::steady_clock::now() + std::chrono::duration<double>(timeout);

  {
    std::unique_lock lock(waitMutex);
    while (!data->waitPredicate) {
      if (data->waitCond.wait_until(lock, timeoutTime) ==
          std::cv_status::timeout) {
        timedOut = true;
        break;
      }
    }
  }

  // Cancel our callback
  SimDIOData[digitalIndex].value.CancelCallback(uid);
  (void)synchronousInterruptHandles->Free(dataHandle);

  // Check for what to return
  if (timedOut) {
    return WaitResult::Timeout;
  }
  // We know the value has changed because we would've timed out otherwise.
  // If the current state is true, the previous state was false, so this is a
  // rising edge. Otherwise, it's a falling edge.
  if (interrupt->currentState) {
    // Set our return value and our timestamps
    interrupt->risingTimestamp = hal::GetFPGATime();
    return 1 << (interrupt->index);
  } else {
    interrupt->fallingTimestamp = hal::GetFPGATime();
    return 1 << (8 + interrupt->index);
  }
}

static int64_t WaitForInterruptAnalog(HAL_InterruptHandle handle,
                                      Interrupt* interrupt, double timeout,
                                      bool ignorePrevious) {
  auto data = std::make_shared<SynchronousWaitData>();

  auto dataHandle = synchronousInterruptHandles->Allocate(data);
  if (dataHandle == HAL_kInvalidHandle) {
    // Error allocating data
    return WaitResult::Timeout;
  }

  data->waitPredicate = false;
  data->interruptHandle = handle;

  int32_t status = 0;
  interrupt->currentState = GetAnalogTriggerValue(interrupt->portHandle,
                                                  interrupt->trigType, &status);

  if (status != 0) {
    return WaitResult::Timeout;
  }

  int32_t analogIndex =
      GetAnalogTriggerInputIndex(interrupt->portHandle, &status);

  if (status != 0) {
    return WaitResult::Timeout;
  }

  int32_t uid = SimAnalogInData[analogIndex].voltage.RegisterCallback(
      &ProcessInterruptAnalogSynchronous,
      reinterpret_cast<void*>(static_cast<uintptr_t>(dataHandle)), false);

  bool timedOut = false;

  wpi::mutex waitMutex;

  auto timeoutTime =
      std::chrono::steady_clock::now() + std::chrono::duration<double>(timeout);

  {
    std::unique_lock lock(waitMutex);
    while (!data->waitPredicate) {
      if (data->waitCond.wait_until(lock, timeoutTime) ==
          std::cv_status::timeout) {
        timedOut = true;
        break;
      }
    }
  }

  // Cancel our callback
  SimAnalogInData[analogIndex].voltage.CancelCallback(uid);
  (void)synchronousInterruptHandles->Free(dataHandle);

  // Check for what to return
  if (timedOut) {
    return WaitResult::Timeout;
  }
  // We know the value has changed because we would've timed out otherwise.
  // If the current state is true, the previous state was false, so this is a
  // rising edge. Otherwise, it's a falling edge.
  if (interrupt->currentState) {
    // Set our return value and our timestamps
    interrupt->risingTimestamp = hal::GetFPGATime();
    return 1 << (interrupt->index);
  } else {
    interrupt->fallingTimestamp = hal::GetFPGATime();
    return 1 << (8 + interrupt->index);
  }
}

int64_t HAL_WaitForInterrupt(HAL_InterruptHandle interruptHandle,
                             double timeout, HAL_Bool ignorePrevious,
                             int32_t* status) {
  auto interrupt = interruptHandles->Get(interruptHandle);
  if (interrupt == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return WaitResult::Timeout;
  }

  if (interrupt->isAnalog) {
    return WaitForInterruptAnalog(interruptHandle, interrupt.get(), timeout,
                                  ignorePrevious);
  } else {
    return WaitForInterruptDigital(interruptHandle, interrupt.get(), timeout,
                                   ignorePrevious);
  }
}

int64_t HAL_WaitForMultipleInterrupts(HAL_InterruptHandle interruptHandle,
                                      int64_t mask, double timeout,
                                      HAL_Bool ignorePrevious,
                                      int32_t* status) {
  // TODO make this properly work, will require a decent rewrite
  auto interrupt = interruptHandles->Get(interruptHandle);
  if (interrupt == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return WaitResult::Timeout;
  }

  if (interrupt->isAnalog) {
    return WaitForInterruptAnalog(interruptHandle, interrupt.get(), timeout,
                                  ignorePrevious);
  } else {
    return WaitForInterruptDigital(interruptHandle, interrupt.get(), timeout,
                                   ignorePrevious);
  }
}

int64_t HAL_ReadInterruptRisingTimestamp(HAL_InterruptHandle interruptHandle,
                                         int32_t* status) {
  auto interrupt = interruptHandles->Get(interruptHandle);
  if (interrupt == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return interrupt->risingTimestamp;
}
int64_t HAL_ReadInterruptFallingTimestamp(HAL_InterruptHandle interruptHandle,
                                          int32_t* status) {
  auto interrupt = interruptHandles->Get(interruptHandle);
  if (interrupt == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return interrupt->fallingTimestamp;
}
void HAL_RequestInterrupts(HAL_InterruptHandle interruptHandle,
                           HAL_Handle digitalSourceHandle,
                           HAL_AnalogTriggerType analogTriggerType,
                           int32_t* status) {
  auto interrupt = interruptHandles->Get(interruptHandle);
  if (interrupt == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

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

  interrupt->isAnalog = routingAnalogTrigger;
  interrupt->trigType = analogTriggerType;
  interrupt->portHandle = digitalSourceHandle;
}

void HAL_SetInterruptUpSourceEdge(HAL_InterruptHandle interruptHandle,
                                  HAL_Bool risingEdge, HAL_Bool fallingEdge,
                                  int32_t* status) {
  auto interrupt = interruptHandles->Get(interruptHandle);
  if (interrupt == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  interrupt->fireOnDown = fallingEdge;
  interrupt->fireOnUp = risingEdge;
}

void HAL_ReleaseWaitingInterrupt(HAL_InterruptHandle interruptHandle,
                                 int32_t* status) {
  auto interrupt = interruptHandles->Get(interruptHandle);
  if (interrupt == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  synchronousInterruptHandles->ForEach(
      [interruptHandle](SynchronousWaitDataHandle handle,
                        SynchronousWaitData* data) {
        if (data->interruptHandle == interruptHandle) {
          data->waitPredicate = true;
          data->waitCond.notify_all();
        }
      });
}
}  // extern "C"
