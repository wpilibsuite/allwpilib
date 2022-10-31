// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/roborio/InterruptManager.h"

#include <fmt/format.h>

#include "FPGACalls.h"
#include "HALInternal.h"
#include "dlfcn.h"
#include "hal/Errors.h"

using namespace hal;

InterruptManager& InterruptManager::GetInstance() {
  static InterruptManager manager;
  return manager;
}

void InterruptManager::Initialize(tSystemInterface* baseSystem) {
  auto& manager = GetInstance();
  manager.fpgaSession = baseSystem->getHandle();
}

NiFpga_IrqContext InterruptManager::GetContext() noexcept {
  NiFpga_IrqContext context;
  HAL_NiFpga_ReserveIrqContext(fpgaSession, &context);
  return context;
}

void InterruptManager::ReleaseContext(NiFpga_IrqContext context) noexcept {
  HAL_NiFpga_UnreserveIrqContext(fpgaSession, context);
}

uint32_t InterruptManager::WaitForInterrupt(NiFpga_IrqContext context,
                                            uint32_t mask, bool ignorePrevious,
                                            uint32_t timeoutMs,
                                            int32_t* status) {
  {
    // Make sure we can safely use this
    std::scoped_lock lock(currentMaskMutex);
    if ((currentMask & mask) != 0) {
      *status = PARAMETER_OUT_OF_RANGE;
      hal::SetLastError(
          status, fmt::format("Interrupt mask {} has bits {} already in use",
                              mask, (currentMask & mask)));
      return 0;
    }
    currentMask |= mask;
  }

  if (ignorePrevious) {
    HAL_NiFpga_AcknowledgeIrqs(fpgaSession, mask);
  }

  uint32_t irqsAsserted = 0;
  NiFpga_Bool timedOut = 0;
  *status = HAL_NiFpga_WaitOnIrqs(fpgaSession, context, mask, timeoutMs,
                                  &irqsAsserted, &timedOut);

  if (!timedOut) {
    HAL_NiFpga_AcknowledgeIrqs(fpgaSession, irqsAsserted);
  }

  {
    std::scoped_lock lock(currentMaskMutex);
    currentMask &= ~mask;
  }

  return irqsAsserted;
}
