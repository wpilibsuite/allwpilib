// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cstdio>
#include <memory>

#include <hal/Extensions.h>

#include "HALSimWSClient.h"

using namespace wpilibws;

static std::unique_ptr<HALSimWSClient> gClient;

extern "C" {
#if defined(WIN32) || defined(_WIN32)
__declspec(dllexport)
#endif

    int HALSIM_InitExtension(void) {
  std::puts("HALSim WS Client Extension Initializing");

  HAL_OnShutdown(nullptr, [](void*) { gClient.reset(); });

  gClient = std::make_unique<HALSimWSClient>();
  if (!gClient->Initialize()) {
    return -1;
  }

  std::puts("HALSim WS Client Extension Initialized");
  return 0;
}

}  // extern "C"
