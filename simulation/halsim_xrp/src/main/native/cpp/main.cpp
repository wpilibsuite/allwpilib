// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cstdio>
#include <memory>

#include <hal/Extensions.h>

#include "HALSimXRPClient.h"

#if defined(Win32) || defined(_WIN32)
#pragma comment(lib, "Ws2_32.lib")
#endif

using namespace wpilibxrp;

static std::unique_ptr<HALSimXRPClient> gClient;

/*--------------------------------------------------------------------------
** Main Entry Point. Start up the listening threads
**------------------------------------------------------------------------*/
extern "C" {
#if defined(WIN32) || defined(_WIN32)
__declspec(dllexport)
#endif

int HALSIM_InitExtension(void) {
  std::puts("HALSim XRP Extension Initializing");

  HAL_OnShutdown(nullptr, [](void*) { gClient.reset(); });

  gClient = std::make_unique<HALSimXRPClient>();
  if (!gClient->Initialize()) {
    return -1;
  }

  std::puts("HALSim XRP Extension Initialized");
  return 0;
}

}  // extern "C"
