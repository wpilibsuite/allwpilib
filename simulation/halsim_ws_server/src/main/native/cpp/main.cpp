/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <memory>

#include <hal/Extensions.h>
#include <wpi/raw_ostream.h>

#include "HALSimWSServer.h"

using namespace std::placeholders;
using namespace wpilibws;

static std::unique_ptr<HALSimWSServer> gServer;

extern "C" {
#if defined(WIN32) || defined(_WIN32)
__declspec(dllexport)
#endif
    int HALSIM_InitExtension(void) {
  wpi::outs() << "Websocket WS Server Initializing.\n";

  HAL_OnShutdown(nullptr, [](void*) { gServer.reset(); });

  gServer = std::make_unique<HALSimWSServer>();
  if (!gServer->Initialize()) {
    return -1;
  }

  wpi::outs() << "Websocket WS Server Initialized!\n";
  return 0;
}
}  // extern "C"
