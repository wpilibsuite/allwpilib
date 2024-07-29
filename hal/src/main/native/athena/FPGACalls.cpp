// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "FPGACalls.h"

#include <cerrno>

#include "dlfcn.h"
#include "hal/Errors.h"

static void* NiFpgaLibrary = nullptr;

namespace hal {
HAL_NiFpga_ReserveIrqContextFunc HAL_NiFpga_ReserveIrqContext;
HAL_NiFpga_UnreserveIrqContextFunc HAL_NiFpga_UnreserveIrqContext;
HAL_NiFpga_WaitOnIrqsFunc HAL_NiFpga_WaitOnIrqs;
HAL_NiFpga_AcknowledgeIrqsFunc HAL_NiFpga_AcknowledgeIrqs;
HAL_NiFpga_OpenHmbFunc HAL_NiFpga_OpenHmb;
HAL_NiFpga_CloseHmbFunc HAL_NiFpga_CloseHmb;

namespace init {
int InitializeFPGA() {
  NiFpgaLibrary = dlopen("libNiFpga.so", RTLD_LAZY);
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
  HAL_NiFpga_OpenHmb = reinterpret_cast<HAL_NiFpga_OpenHmbFunc>(
      dlsym(NiFpgaLibrary, "NiFpgaDll_OpenHmb"));
  HAL_NiFpga_CloseHmb = reinterpret_cast<HAL_NiFpga_CloseHmbFunc>(
      dlsym(NiFpgaLibrary, "NiFpgaDll_CloseHmb"));
#pragma GCC diagnostic pop

  if (HAL_NiFpga_ReserveIrqContext == nullptr ||
      HAL_NiFpga_UnreserveIrqContext == nullptr ||
      HAL_NiFpga_WaitOnIrqs == nullptr ||
      HAL_NiFpga_AcknowledgeIrqs == nullptr || HAL_NiFpga_OpenHmb == nullptr ||
      HAL_NiFpga_CloseHmb == nullptr) {
    HAL_NiFpga_ReserveIrqContext = nullptr;
    HAL_NiFpga_UnreserveIrqContext = nullptr;
    HAL_NiFpga_WaitOnIrqs = nullptr;
    HAL_NiFpga_AcknowledgeIrqs = nullptr;
    HAL_NiFpga_OpenHmb = nullptr;
    HAL_NiFpga_CloseHmb = nullptr;
    dlclose(NiFpgaLibrary);
    NiFpgaLibrary = nullptr;
    return NO_AVAILABLE_RESOURCES;
  }

  return HAL_SUCCESS;
}
}  // namespace init
}  // namespace hal
