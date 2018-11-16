/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/Interrupts.h"

#include <memory>

#include <wpi/SafeThread.h>

#include "DigitalInternal.h"
#include "HALInitializer.h"
#include "PortsInternal.h"
#include "hal/ChipObject.h"
#include "hal/Errors.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/handles/LimitedHandleResource.h"

using namespace hal;

namespace {
// Safe thread to allow callbacks to run on their own thread
class InterruptThread : public wpi::SafeThread {
 public:
  void Main() {
    std::unique_lock<wpi::mutex> lock(m_mutex);
    while (m_active) {
      m_cond.wait(lock, [&] { return !m_active || m_notify; });
      if (!m_active) break;
      m_notify = false;
      HAL_InterruptHandlerFunction handler = m_handler;
      uint32_t mask = m_mask;
      void* param = m_param;
      lock.unlock();  // don't hold mutex during callback execution
      handler(mask, param);
      lock.lock();
    }
  }

  bool m_notify = false;
  HAL_InterruptHandlerFunction m_handler;
  void* m_param;
  uint32_t m_mask;
};

class InterruptThreadOwner : public wpi::SafeThreadOwner<InterruptThread> {
 public:
  void SetFunc(HAL_InterruptHandlerFunction handler, void* param) {
    auto thr = GetThread();
    if (!thr) return;
    thr->m_handler = handler;
    thr->m_param = param;
  }

  void Notify(uint32_t mask) {
    auto thr = GetThread();
    if (!thr) return;
    thr->m_mask = mask;
    thr->m_notify = true;
    thr->m_cond.notify_one();
  }
};

}  // namespace

static void threadedInterruptHandler(uint32_t mask, void* param) {
  static_cast<InterruptThreadOwner*>(param)->Notify(mask);
}

struct Interrupt {
  std::unique_ptr<tInterrupt> anInterrupt;
  std::unique_ptr<tInterruptManager> manager;
  std::unique_ptr<InterruptThreadOwner> threadOwner = nullptr;
  void* param = nullptr;
};

static LimitedHandleResource<HAL_InterruptHandle, Interrupt, kNumInterrupts,
                             HAL_HandleEnum::Interrupt>* interruptHandles;

namespace hal {
namespace init {
void InitialzeInterrupts() {
  static LimitedHandleResource<HAL_InterruptHandle, Interrupt, kNumInterrupts,
                               HAL_HandleEnum::Interrupt>
      iH;
  interruptHandles = &iH;
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
  uint32_t interruptIndex = static_cast<uint32_t>(getHandleIndex(handle));
  // Expects the calling leaf class to allocate an interrupt index.
  anInterrupt->anInterrupt.reset(tInterrupt::create(interruptIndex, status));
  anInterrupt->anInterrupt->writeConfig_WaitForAck(false, status);
  anInterrupt->manager = std::make_unique<tInterruptManager>(
      (1u << interruptIndex) | (1u << (interruptIndex + 8u)), watcher, status);
  return handle;
}

void* HAL_CleanInterrupts(HAL_InterruptHandle interruptHandle,
                          int32_t* status) {
  auto anInterrupt = interruptHandles->Get(interruptHandle);
  interruptHandles->Free(interruptHandle);
  if (anInterrupt == nullptr) {
    return nullptr;
  }
  anInterrupt->manager->enable(status);
  void* param = anInterrupt->param;
  return param;
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

  result = anInterrupt->manager->watch(static_cast<int32_t>(timeout * 1e3),
                                       ignorePrevious, status);

  // Don't report a timeout as an error - the return code is enough to tell
  // that a timeout happened.
  if (*status == -NiFpga_Status_IrqTimeout) {
    *status = NiFpga_Status_Success;
  }

  return result;
}

void HAL_EnableInterrupts(HAL_InterruptHandle interruptHandle,
                          int32_t* status) {
  auto anInterrupt = interruptHandles->Get(interruptHandle);
  if (anInterrupt == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  anInterrupt->manager->enable(status);
}

void HAL_DisableInterrupts(HAL_InterruptHandle interruptHandle,
                           int32_t* status) {
  auto anInterrupt = interruptHandles->Get(interruptHandle);
  if (anInterrupt == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  anInterrupt->manager->disable(status);
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

void HAL_AttachInterruptHandler(HAL_InterruptHandle interruptHandle,
                                HAL_InterruptHandlerFunction handler,
                                void* param, int32_t* status) {
  auto anInterrupt = interruptHandles->Get(interruptHandle);
  if (anInterrupt == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  anInterrupt->manager->registerHandler(handler, param, status);
  anInterrupt->param = param;
}

void HAL_AttachInterruptHandlerThreaded(HAL_InterruptHandle interrupt_handle,
                                        HAL_InterruptHandlerFunction handler,
                                        void* param, int32_t* status) {
  auto anInterrupt = interruptHandles->Get(interrupt_handle);
  if (anInterrupt == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  anInterrupt->threadOwner = std::make_unique<InterruptThreadOwner>();
  anInterrupt->threadOwner->Start();
  anInterrupt->threadOwner->SetFunc(handler, param);

  HAL_AttachInterruptHandler(interrupt_handle, threadedInterruptHandler,
                             anInterrupt->threadOwner.get(), status);

  if (*status != 0) {
    anInterrupt->threadOwner = nullptr;
  }
  anInterrupt->param = param;
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

}  // extern "C"
