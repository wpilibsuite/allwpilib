// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/*----------------------------------------------------------------------------
**  This extension reimplements enough of the WPILIB_Network layer to enable the
**    simulator to communicate with a driver station.  That includes a
**    simple udp layer for communication.
**  The protocol does not appear to be well documented; this implementation
**    is based in part on the Toast ds_comms.cpp by Jaci and in part
**    by the protocol specification given by QDriverStation.
**--------------------------------------------------------------------------*/

#include <sys/types.h>

#include <atomic>
#include <cstdio>
#include <cstring>
#include <exception>
#include <memory>
#include <string_view>

#include <fmt/format.h>

#include "wpi/hal/cpp/MrcLibDs.hpp"

#include "mrclib/ApiVersion.h"
#include "mrclib/SimSystemServer.h"
#include "wpi/hal/DashboardOpMode.hpp"
#include "wpi/hal/Extensions.h"

static std::atomic<bool> gDSConnected = true;

/*----------------------------------------------------------------------------
** Main entry point.  We will start listen threads going, processing
**  against our driver station packet
**--------------------------------------------------------------------------*/
extern "C" {
#if defined(WIN32) || defined(_WIN32)
__declspec(dllexport)
#endif
int HALSIM_InitExtension(void) {
  static bool once = false;

  if (once) {
    std::fputs("Error: cannot invoke HALSIM_InitExtension twice.\n", stderr);
    return -1;
  }
  once = true;

  std::puts("DriverStationSocket Initializing.");

  HAL_RegisterExtension("ds_socket", &gDSConnected);

  // Before initializing, we need to set up the fake system server
    if (!MRC_CHECK_API_VERSION()) {
    fmt::print(
        stderr,
        "Error: MRC API version mismatch. Restarting app and retrying...");

    std::terminate();
  }

  MRC_SimSystemServer_Initialize();

  wpi::hal::ForceRealDs();

  std::puts("DriverStationSocket Initialized!");
  return 0;
}
}  // extern "C"
