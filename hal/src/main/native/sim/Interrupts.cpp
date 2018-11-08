/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
#include "hal/handles/HandlesInternal.h"
#include "hal/handles/LimitedHandleResource.h"
#include "hal/handles/UnlimitedHandleResource.h"
#include "mockdata/AnalogInDataInternal.h"
#include "mockdata/DIODataInternal.h"
#include "mockdata/HAL_Value.h"

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
  bool watcher;
  int64_t risingTimestamp;
  int64_t fallingTimestamp;
  bool previousState;
  bool fireOnUp;
  bool fireOnDown;
  int32_t callbackId;

  void* callbackParam;
  HAL_InterruptHandlerFunction callbackFunction;
};

struct SynchronousWaitData {
  HAL_InterruptHandle interruptHandle;
  wpi::condition_variable waitCond;
  HAL_Bool waitPredicate;
};
}  // namespace

static LimitedHandleResource<HAL_InterruptHandle, Interrupt, kNumInterrupts,
                             HAL_HandleEnum::Interrupt>* interruptHandles;

typedef HAL_Handle SynchronousWaitDataHandle;
static UnlimitedHandleResource<SynchronousWaitDataHandle, SynchronousWaitData,
                               HAL_HandleEnum::Vendor>*
    synchronousInterruptHandles;

namespace hal {
namespace init {
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
}  // namespace init
}  // namespace hal

extern "C" {
HAL_InterruptHandle HAL_InitializeInterrupts(HAL_Bool watcher,
                                             int32_t* status) {
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

  anInterrupt->watcher = watcher;

  return handle;
}
void* HAL_CleanInterrupts(HAL_InterruptHandle interruptHandle,
                          int32_t* status) {
  HAL_DisableInterrupts(interruptHandle, status);
  auto anInterrupt = interruptHandles->Get(interruptHandle);
  interruptHandles->Free(interruptHandle);
  if (anInterrupt == nullptr) {
    return nullptr;
  }
  return anInterrupt->callbackParam;
}

static void ProcessInterruptDigitalSynchronous(const char* name, void* param,
                                               const struct HAL_Value* value) {
  // void* is a SynchronousWaitDataHandle.
  // convert to uintptr_t first, then to handle
  uintptr_t handleTmp = reinterpret_cast<uintptr_t>(param);
  SynchronousWaitDataHandle handle =
      static_cast<SynchronousWaitDataHandle>(handleTmp);
  auto interruptData = synchronousInterruptHandles->Get(handle);
  if (interruptData == nullptr) return;
  auto interrupt = interruptHandles->Get(interruptData->interruptHandle);
  if (interrupt == nullptr) return;
  // Have a valid interrupt
  if (value->type != HAL_Type::HAL_BOOLEAN) return;
  bool retVal = value->data.v_boolean;
  // If no change in interrupt, return;
  if (retVal == interrupt->previousState) return;
  // If its a falling change, and we dont fire on falling return
  if (interrupt->previousState && !interrupt->fireOnDown) return;
  // If its a rising change, and we dont fire on rising return.
  if (!interrupt->previousState && !interrupt->fireOnUp) return;

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
  if (interruptData == nullptr) return;
  auto interrupt = interruptHandles->Get(interruptData->interruptHandle);
  if (interrupt == nullptr) return;
  // Have a valid interrupt
  if (value->type != HAL_Type::HAL_DOUBLE) return;
  int32_t status = 0;
  bool retVal = GetAnalogTriggerValue(interrupt->portHandle,
                                      interrupt->trigType, &status);
  if (status != 0) {
    // Interrupt and Cancel
    interruptData->waitPredicate = true;
    // Pulse interrupt
    interruptData->waitCond.notify_all();
  }
  // If no change in interrupt, return;
  if (retVal == interrupt->previousState) return;
  // If its a falling change, and we dont fire on falling return
  if (interrupt->previousState && !interrupt->fireOnDown) return;
  // If its a rising change, and we dont fire on rising return.
  if (!interrupt->previousState && !interrupt->fireOnUp) return;

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

  if (status != 0) return WaitResult::Timeout;

  interrupt->previousState = SimDIOData[digitalIndex].value;

  int32_t uid = SimDIOData[digitalIndex].value.RegisterCallback(
      &ProcessInterruptDigitalSynchronous,
      reinterpret_cast<void*>(static_cast<uintptr_t>(dataHandle)), false);

  bool timedOut = false;

  wpi::mutex waitMutex;

  auto timeoutTime =
      std::chrono::steady_clock::now() + std::chrono::duration<double>(timeout);

  {
    std::unique_lock<wpi::mutex> lock(waitMutex);
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
  synchronousInterruptHandles->Free(dataHandle);

  // Check for what to return
  if (timedOut) return WaitResult::Timeout;
  // True => false, Falling
  if (interrupt->previousState) {
    // Set our return value and our timestamps
    interrupt->fallingTimestamp = hal::GetFPGATime();
    return 1 << (8 + interrupt->index);
  } else {
    interrupt->risingTimestamp = hal::GetFPGATime();
    return 1 << (interrupt->index);
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
  interrupt->previousState = GetAnalogTriggerValue(
      interrupt->portHandle, interrupt->trigType, &status);

  if (status != 0) return WaitResult::Timeout;

  int32_t analogIndex =
      GetAnalogTriggerInputIndex(interrupt->portHandle, &status);

  if (status != 0) return WaitResult::Timeout;

  int32_t uid = SimAnalogInData[analogIndex].voltage.RegisterCallback(
      &ProcessInterruptAnalogSynchronous,
      reinterpret_cast<void*>(static_cast<uintptr_t>(dataHandle)), false);

  bool timedOut = false;

  wpi::mutex waitMutex;

  auto timeoutTime =
      std::chrono::steady_clock::now() + std::chrono::duration<double>(timeout);

  {
    std::unique_lock<wpi::mutex> lock(waitMutex);
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
  synchronousInterruptHandles->Free(dataHandle);

  // Check for what to return
  if (timedOut) return WaitResult::Timeout;
  // True => false, Falling
  if (interrupt->previousState) {
    // Set our return value and our timestamps
    interrupt->fallingTimestamp = hal::GetFPGATime();
    return 1 << (8 + interrupt->index);
  } else {
    interrupt->risingTimestamp = hal::GetFPGATime();
    return 1 << (interrupt->index);
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

  // Check to make sure we are actually an interrupt in synchronous mode
  if (!interrupt->watcher) {
    *status = NiFpga_Status_InvalidParameter;
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

static void ProcessInterruptDigitalAsynchronous(const char* name, void* param,
                                                const struct HAL_Value* value) {
  // void* is a HAL handle
  // convert to uintptr_t first, then to handle
  uintptr_t handleTmp = reinterpret_cast<uintptr_t>(param);
  HAL_InterruptHandle handle = static_cast<HAL_InterruptHandle>(handleTmp);
  auto interrupt = interruptHandles->Get(handle);
  if (interrupt == nullptr) return;
  // Have a valid interrupt
  if (value->type != HAL_Type::HAL_BOOLEAN) return;
  bool retVal = value->data.v_boolean;
  // If no change in interrupt, return;
  if (retVal == interrupt->previousState) return;
  int32_t mask = 0;
  if (interrupt->previousState) {
    interrupt->previousState = retVal;
    interrupt->fallingTimestamp = hal::GetFPGATime();
    mask = 1 << (8 + interrupt->index);
    if (!interrupt->fireOnDown) return;
  } else {
    interrupt->previousState = retVal;
    interrupt->risingTimestamp = hal::GetFPGATime();
    mask = 1 << (interrupt->index);
    if (!interrupt->fireOnUp) return;
  }

  // run callback
  auto callback = interrupt->callbackFunction;
  if (callback == nullptr) return;
  callback(mask, interrupt->callbackParam);
}

static void ProcessInterruptAnalogAsynchronous(const char* name, void* param,
                                               const struct HAL_Value* value) {
  // void* is a HAL handle
  // convert to intptr_t first, then to handle
  uintptr_t handleTmp = reinterpret_cast<uintptr_t>(param);
  HAL_InterruptHandle handle = static_cast<HAL_InterruptHandle>(handleTmp);
  auto interrupt = interruptHandles->Get(handle);
  if (interrupt == nullptr) return;
  // Have a valid interrupt
  if (value->type != HAL_Type::HAL_DOUBLE) return;
  int32_t status = 0;
  bool retVal = GetAnalogTriggerValue(interrupt->portHandle,
                                      interrupt->trigType, &status);
  if (status != 0) return;
  // If no change in interrupt, return;
  if (retVal == interrupt->previousState) return;
  int mask = 0;
  if (interrupt->previousState) {
    interrupt->previousState = retVal;
    interrupt->fallingTimestamp = hal::GetFPGATime();
    if (!interrupt->fireOnDown) return;
    mask = 1 << (8 + interrupt->index);
  } else {
    interrupt->previousState = retVal;
    interrupt->risingTimestamp = hal::GetFPGATime();
    if (!interrupt->fireOnUp) return;
    mask = 1 << (interrupt->index);
  }

  // run callback
  auto callback = interrupt->callbackFunction;
  if (callback == nullptr) return;
  callback(mask, interrupt->callbackParam);
}

static void EnableInterruptsDigital(HAL_InterruptHandle handle,
                                    Interrupt* interrupt) {
  int32_t status = 0;
  int32_t digitalIndex = GetDigitalInputChannel(interrupt->portHandle, &status);
  if (status != 0) return;

  interrupt->previousState = SimDIOData[digitalIndex].value;

  int32_t uid = SimDIOData[digitalIndex].value.RegisterCallback(
      &ProcessInterruptDigitalAsynchronous,
      reinterpret_cast<void*>(static_cast<uintptr_t>(handle)), false);
  interrupt->callbackId = uid;
}

static void EnableInterruptsAnalog(HAL_InterruptHandle handle,
                                   Interrupt* interrupt) {
  int32_t status = 0;
  int32_t analogIndex =
      GetAnalogTriggerInputIndex(interrupt->portHandle, &status);
  if (status != 0) return;

  status = 0;
  interrupt->previousState = GetAnalogTriggerValue(
      interrupt->portHandle, interrupt->trigType, &status);
  if (status != 0) return;

  int32_t uid = SimAnalogInData[analogIndex].voltage.RegisterCallback(
      &ProcessInterruptAnalogAsynchronous,
      reinterpret_cast<void*>(static_cast<uintptr_t>(handle)), false);
  interrupt->callbackId = uid;
}

void HAL_EnableInterrupts(HAL_InterruptHandle interruptHandle,
                          int32_t* status) {
  auto interrupt = interruptHandles->Get(interruptHandle);
  if (interrupt == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  // If we have not had a callback set, error out
  if (interrupt->callbackFunction == nullptr) {
    *status = INCOMPATIBLE_STATE;
    return;
  }

  // EnableInterrupts has already been called
  if (interrupt->callbackId >= 0) {
    // We can double enable safely.
    return;
  }

  if (interrupt->isAnalog) {
    EnableInterruptsAnalog(interruptHandle, interrupt.get());
  } else {
    EnableInterruptsDigital(interruptHandle, interrupt.get());
  }
}
void HAL_DisableInterrupts(HAL_InterruptHandle interruptHandle,
                           int32_t* status) {
  auto interrupt = interruptHandles->Get(interruptHandle);
  if (interrupt == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  // No need to disable if we are already disabled
  if (interrupt->callbackId < 0) return;

  if (interrupt->isAnalog) {
    // Do analog
    int32_t status = 0;
    int32_t analogIndex =
        GetAnalogTriggerInputIndex(interrupt->portHandle, &status);
    if (status != 0) return;
    SimAnalogInData[analogIndex].voltage.CancelCallback(interrupt->callbackId);
  } else {
    int32_t status = 0;
    int32_t digitalIndex =
        GetDigitalInputChannel(interrupt->portHandle, &status);
    if (status != 0) return;
    SimDIOData[digitalIndex].value.CancelCallback(interrupt->callbackId);
  }
  interrupt->callbackId = -1;
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
void HAL_AttachInterruptHandler(HAL_InterruptHandle interruptHandle,
                                HAL_InterruptHandlerFunction handler,
                                void* param, int32_t* status) {
  auto interrupt = interruptHandles->Get(interruptHandle);
  if (interrupt == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  interrupt->callbackFunction = handler;
  interrupt->callbackParam = param;
}

void HAL_AttachInterruptHandlerThreaded(HAL_InterruptHandle interruptHandle,
                                        HAL_InterruptHandlerFunction handler,
                                        void* param, int32_t* status) {
  HAL_AttachInterruptHandler(interruptHandle, handler, param, status);
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
}  // extern "C"
