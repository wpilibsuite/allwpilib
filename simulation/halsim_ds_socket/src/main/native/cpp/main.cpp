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

#include "mrclib/ApiVersion.h"
#include "mrclib/DsComms.hpp"
#include "mrclib/SimSystemServer.h"
#include "wpi/hal/DashboardOpMode.hpp"
#include "wpi/hal/DriverStation.h"
#include "wpi/hal/Extensions.h"
#include "wpi/hal/cpp/MrcLibDs.hpp"
#include "wpi/hal/simulation/DriverStationData.h"
#include "wpi/util/SafeThread.hpp"

static void WriteSimBackend();

namespace {
class Thread : public wpi::util::SafeThread {
 public:
  Thread() {}
  void Main() override;
};

void Thread::Main() {
  wpi::util::Event event{false, false};
  HAL_ProvideNewDataEventHandle(event.GetHandle());

  while (m_active) {
    bool timedOut = false;
    wpi::util::WaitForObject(event.GetHandle(), 1.0, &timedOut);
    WriteSimBackend();
  }

  HAL_RemoveNewDataEventHandle(event.GetHandle());
}
}  // namespace

static std::atomic<bool> gDSConnected = true;

static void WriteSimBackend() {
  MRC_Joysticks joysticks;
  MRC_ControlData controlData;

  int32_t status =
      MRC_DsComms_GetControlDataWithJoysticks(&controlData, &joysticks);
  if (status != 0) {
    return;
  }

  printf("Control data status: 0x%x\n", controlData.controlFlags);
  fflush(stdout);

  HALSIM_SetDriverStationOpMode(controlData.currentOpMode);
  HALSIM_SetDriverStationAllianceStationId(static_cast<HAL_AllianceStationID>(
      mrclib::GetAlliance(controlData.controlFlags) + 1));
  HALSIM_SetDriverStationMatchTime(controlData.matchTime);
  HALSIM_SetDriverStationRobotMode(static_cast<HAL_RobotMode>(
      mrclib::GetRobotMode(controlData.controlFlags)));
  HALSIM_SetDriverStationDsAttached(
      mrclib::GetDsConnected(controlData.controlFlags));
  HALSIM_SetDriverStationFmsAttached(
      mrclib::GetFmsConnected(controlData.controlFlags));
  HALSIM_SetDriverStationEnabled(mrclib::GetEnabled(controlData.controlFlags));
  HALSIM_SetDriverStationEStop(mrclib::GetEStop(controlData.controlFlags));

  HAL_GameData gameData;
  auto gameDataSize = controlData.gameDataLength;
  if (gameDataSize > MRCLIB_MAX_GAMEDATA_LENGTH) {
    gameDataSize = MRCLIB_MAX_GAMEDATA_LENGTH;
  }
  std::memcpy(gameData.gameData, controlData.gameData, gameDataSize);
  gameData.gameData[gameDataSize] = '\0';
  HALSIM_SetGameData(&gameData);

  HALSIM_NotifyDriverStationNewData();
}

static void SetupBackend() {
  static wpi::util::SafeThreadOwner<Thread> thread;
  thread.Start();
}

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

  WriteSimBackend();

  SetupBackend();

  std::puts("DriverStationSocket Initialized!");
  return 0;
}
}  // extern "C"
