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

#include <algorithm>
#include <atomic>
#include <cstdio>
#include <cstring>
#include <exception>
#include <format>
#include <memory>
#include <string_view>

#include "mrclib/ApiVersion.h"
#include "mrclib/DsComms.hpp"
#include "mrclib/SimSystemServer.h"
#include "wpi/hal/DashboardOpMode.hpp"
#include "wpi/hal/DriverStation.h"
#include "wpi/hal/Extensions.h"
#include "wpi/hal/cpp/MrcLibDs.hpp"
#include "wpi/hal/simulation/DriverStationData.h"
#include "wpi/util/SafeThread.hpp"
#include "wpi/util/print.hpp"

static_assert(MRCLIB_MAX_AXES == HAL_MAX_JOYSTICK_AXES);
static_assert(MRCLIB_MAX_POVS == HAL_MAX_JOYSTICK_POVS);
static_assert(MRCLIB_MAX_JOYSTICKS == HAL_MAX_JOYSTICKS);
static_assert(MRCLIB_MAX_TOUCHPADS == HAL_MAX_JOYSTICK_TOUCHPADS);
static_assert(MRCLIB_MAX_TOUCHPAD_FINGERS == HAL_MAX_JOYSTICK_TOUCHPAD_FINGERS);
static_assert(MRCLIB_MAX_JOYSTICK_NAME_LENGTH ==
              sizeof(HAL_JoystickDescriptor::name) - 1);

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

  for (int i = 0; i < MRCLIB_MAX_JOYSTICKS; ++i) {
    HAL_JoystickAxes axes{};
    HAL_JoystickPOVs povs{};
    HAL_JoystickButtons buttons{};
    HAL_JoystickTouchpads touchpads{};

    if (i < joysticks.count) {
      auto& joystick = joysticks.joysticks[i];

      axes.available = joystick.availableAxes;
      for (size_t j = 0; j < MRCLIB_MAX_AXES; ++j) {
        auto raw = joystick.axes[j];
        axes.raw[j] = raw;
        if (raw < 0) {
          axes.axes[j] = raw / 32768.0f;
        } else {
          axes.axes[j] = raw / 32767.0f;
        }
      }

      povs.available = joystick.availablePovs;
      for (size_t j = 0; j < MRCLIB_MAX_POVS; ++j) {
        povs.povs[j] = static_cast<HAL_JoystickPOV>(joystick.povs[j]);
      }

      buttons.available = joystick.availableButtons;
      buttons.buttons = joystick.buttons;

      touchpads.count = joystick.touchpadCount;
      for (size_t j = 0; j < MRCLIB_MAX_TOUCHPADS; ++j) {
        touchpads.touchpads[j].count = joystick.touchpads[j].count;
        for (size_t k = 0; k < MRCLIB_MAX_TOUCHPAD_FINGERS; ++k) {
          auto& finger = joystick.touchpads[j].fingers[k];
          touchpads.touchpads[j].fingers[k].down = finger.down ? 1 : 0;
          touchpads.touchpads[j].fingers[k].x = finger.x / 65535.0f;
          touchpads.touchpads[j].fingers[k].y = finger.y / 65535.0f;
        }
      }
    }

    HALSIM_SetJoystickAxes(i, &axes);
    HALSIM_SetJoystickPOVs(i, &povs);
    HALSIM_SetJoystickButtons(i, &buttons);
    HALSIM_SetJoystickTouchpads(i, &touchpads);
  }

  MRC_JoystickDescriptors descriptors;
  status = MRC_DsComms_GetJoystickDescriptors(&descriptors);
  if (status == 0) {
    for (int i = 0; i < MRCLIB_MAX_JOYSTICKS; ++i) {
      HAL_JoystickDescriptor descriptor{};

      if (i < descriptors.count) {
        auto& mrcDescriptor = descriptors.descriptors[i];

        descriptor.isGamepad = mrcDescriptor.isGamepad ? 1 : 0;
        descriptor.gamepadType = mrcDescriptor.gamepadType;
        descriptor.supportedOutputs = mrcDescriptor.supportedOutputs;

        auto nameLength = std::min<size_t>(mrcDescriptor.nameLength,
                                           sizeof(descriptor.name) - 1);
        std::memcpy(descriptor.name, mrcDescriptor.name, nameLength);
        descriptor.name[nameLength] = '\0';
      }

      HALSIM_SetJoystickDescriptor(i, &descriptor);
    }
  }

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
    wpi::util::print(
        stderr,
        "Error: MRC API version mismatch. Restarting app and retrying...");

    std::terminate();
  }

  MRC_SimSystemServer_Initialize();

  wpi::hal::ForceDsInstance(wpi::hal::GetMrcLibDs());

  WriteSimBackend();

  SetupBackend();

  std::puts("DriverStationSocket Initialized!");
  return 0;
}
}  // extern "C"
