// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "InterruptManager.h"

#include <fmt/format.h>

#include "HALInternal.h"
#include "dlfcn.h"
#include "hal/Errors.h"

// Low level FPGA calls
using HAL_NiFpga_ReserveIrqContextFunc =
    NiFpga_Status (*)(NiFpga_Session session, NiFpga_IrqContext* context);

static HAL_NiFpga_ReserveIrqContextFunc HAL_NiFpga_ReserveIrqContext;

using HAL_NiFpga_UnreserveIrqContextFunc =
    NiFpga_Status (*)(NiFpga_Session session, NiFpga_IrqContext context);

static HAL_NiFpga_UnreserveIrqContextFunc HAL_NiFpga_UnreserveIrqContext;

using HAL_NiFpga_WaitOnIrqsFunc = NiFpga_Status (*)(
    NiFpga_Session session, NiFpga_IrqContext context, uint32_t irqs,
    uint32_t timeout, uint32_t* irqsAsserted, NiFpga_Bool* timedOut);

static HAL_NiFpga_WaitOnIrqsFunc HAL_NiFpga_WaitOnIrqs;

using HAL_NiFpga_AcknowledgeIrqsFunc = NiFpga_Status (*)(NiFpga_Session session,
                                                         uint32_t irqs);

static HAL_NiFpga_AcknowledgeIrqsFunc HAL_NiFpga_AcknowledgeIrqs;

static void* NiFpgaLibrary = nullptr;

using namespace hal;

InterruptManager& InterruptManager::Get() {
  static InterruptManager manager;
  return manager;
}

int32_t InterruptManager::Initialize(tSystemInterface* baseSystem) {
  auto& manager = Get();
  manager.fpgaSession = baseSystem->getHandle();

  NiFpgaLibrary = dlopen("libNiFpga.so", 0);
  if (!NiFpgaLibrary) {
    return errno;
  }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
  HAL_NiFpga_ReserveIrqContext =
      reinterpret_cast<HAL_NiFpga_ReserveIrqContextFunc>(
          dlsym(NiFpgaLibrary, "NiFpgaDll_ReserveIrqContext"));
  HAL_NiFpga_UnreserveIrqContext =
      reinterpret_cast<HAL_NiFpga_UnreserveIrqContextFunc>(
          dlsym(NiFpgaLibrary, "NiFpgaDll_UnreserveIrqContext"));
  HAL_NiFpga_WaitOnIrqs = reinterpret_cast<HAL_NiFpga_WaitOnIrqsFunc>(
      dlsym(NiFpgaLibrary, "NiFpgaDll_WaitOnIrqs"));
  HAL_NiFpga_AcknowledgeIrqs = reinterpret_cast<HAL_NiFpga_AcknowledgeIrqsFunc>(
      dlsym(NiFpgaLibrary, "NiFpgaDll_AcknowledgeIrqs"));
#pragma GCC diagnostic pop

  // TODO Actually return errors

  return HAL_SUCCESS;
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
