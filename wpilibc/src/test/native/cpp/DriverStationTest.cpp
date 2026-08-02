// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <array>
#include <cstdio>
#include <string>
#include <string_view>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "wpi/driverstation/Joystick.hpp"
#include "wpi/driverstation/internal/DriverStationBackend.hpp"
#include "wpi/hal/simulation/DriverStationData.h"
#include "wpi/simulation/AlertSim.hpp"
#include "wpi/simulation/DriverStationSim.hpp"
#include "wpi/simulation/SimHooks.hpp"
#include "wpi/util/Alert.hpp"

namespace {

class StderrCapture {
 public:
  StderrCapture() {
    std::fflush(stderr);
    m_file = std::tmpfile();
    REQUIRE(m_file != nullptr);
    m_originalStderr = Dup(Fileno(stderr));
    REQUIRE(m_originalStderr >= 0);
    REQUIRE(Dup2(Fileno(m_file), Fileno(stderr)) >= 0);
  }

  StderrCapture(const StderrCapture&) = delete;
  StderrCapture& operator=(const StderrCapture&) = delete;

  ~StderrCapture() {
    if (m_originalStderr >= 0) {
      std::fflush(stderr);
      Dup2(m_originalStderr, Fileno(stderr));
      Close(m_originalStderr);
    }
    if (m_file) {
      std::fclose(m_file);
    }
  }

  std::string Stop() {
    std::fflush(stderr);
    REQUIRE(Dup2(m_originalStderr, Fileno(stderr)) >= 0);
    Close(m_originalStderr);
    m_originalStderr = -1;

    std::rewind(m_file);
    std::string output;
    std::array<char, 1024> buffer;
    while (auto count = std::fread(buffer.data(), 1, buffer.size(), m_file)) {
      output.append(buffer.data(), count);
    }
    return output;
  }

 private:
  static int Fileno(FILE* file) {
#ifdef _WIN32
    return _fileno(file);
#else
    return fileno(file);
#endif
  }

  static int Dup(int fd) {
#ifdef _WIN32
    return _dup(fd);
#else
    return dup(fd);
#endif
  }

  static int Dup2(int source, int dest) {
#ifdef _WIN32
    return _dup2(source, dest);
#else
    return dup2(source, dest);
#endif
  }

  static void Close(int fd) {
#ifdef _WIN32
    _close(fd);
#else
    close(fd);
#endif
  }

  FILE* m_file = nullptr;
  int m_originalStderr = -1;
};

static void ResetJoystickAlerts() {
  wpi::sim::DriverStationSim::ResetData();
  HALSIM_SetJoystickButtonsAvailable(0, 0);
  HALSIM_SetJoystickAxesAvailable(0, 0);
  HALSIM_SetJoystickPOVsAvailable(0, 0);
  wpi::sim::DriverStationSim::SetFmsAttached(false);
  wpi::sim::DriverStationSim::NotifyNewData();
  wpi::internal::DriverStationBackend::SilenceJoystickConnectionAlert(true);
  wpi::sim::AlertSim::ResetData();
  wpi::internal::DriverStationBackend::SilenceJoystickConnectionAlert(false);
}

static bool IsDriverStationAlertActive(std::string_view id,
                                       std::string_view text,
                                       wpi::util::Alert::Level level) {
  for (const auto& alert : wpi::sim::AlertSim::GetActive()) {
    if (alert.group == "DriverStation" && alert.id == id &&
        alert.text == text && alert.level == level) {
      return true;
    }
  }

  return false;
}

static bool IsJoystickDisconnectedAlertActive(int stick) {
  std::string expectedText =
      "Joystick on port " + std::to_string(stick) +
      " not available, check if all controllers are plugged in";

  return IsDriverStationAlertActive(
      "joystick" + std::to_string(stick) + "Disconnected", expectedText,
      wpi::util::Alert::Level::HIGH);
}

}  // namespace

TEST_CASE("DriverStation joystick connected", "[wpilibc][driverstation]") {
  wpi::sim::DriverStationSim::ResetData();
  wpi::sim::DriverStationSim::NotifyNewData();

  auto [axes, buttons, povs, expected] =
      GENERATE(table<int, int, int, bool>({{0, 0, 0, false},
                                           {1, 0, 0, true},
                                           {0, 1, 0, true},
                                           {0, 0, 1, true},
                                           {1, 1, 1, true},
                                           {4, 10, 1, true}}));

  wpi::sim::DriverStationSim::SetJoystickAxesMaximumIndex(1, axes);
  wpi::sim::DriverStationSim::SetJoystickButtonsMaximumIndex(1, buttons);
  wpi::sim::DriverStationSim::SetJoystickPOVsMaximumIndex(1, povs);
  wpi::sim::DriverStationSim::NotifyNewData();

  REQUIRE(expected ==
          wpi::internal::DriverStationBackend::IsJoystickConnected(1));
}

TEST_CASE("DriverStation joystick connection alerts",
          "[wpilibc][driverstation]") {
  ResetJoystickAlerts();

  auto [fmsAttached, silenceAlerts, expectedSilenced, expectedAlert] =
      GENERATE(table<bool, bool, bool, bool>({{false, true, true, false},
                                              {false, false, false, true},
                                              {true, true, false, true},
                                              {true, false, false, true}}));

  StderrCapture capture;

  wpi::sim::DriverStationSim::SetFmsAttached(fmsAttached);
  wpi::sim::DriverStationSim::NotifyNewData();
  wpi::internal::DriverStationBackend::SilenceJoystickConnectionAlert(
      silenceAlerts);

  wpi::Joystick joystick{0};
  joystick.GetHID().GetRawButton(1);

  wpi::sim::StepTiming(1_s);
  CHECK(wpi::internal::DriverStationBackend::
            IsJoystickConnectionAlertSilenced() == expectedSilenced);
  CHECK(IsJoystickDisconnectedAlertActive(0) == expectedAlert);
  CHECK(capture.Stop() == "");

  ResetJoystickAlerts();
}

TEST_CASE("DriverStation joystick resource alerts",
          "[wpilibc][driverstation]") {
  ResetJoystickAlerts();

  HALSIM_SetJoystickButtonsAvailable(0, 1);
  HALSIM_SetJoystickAxesAvailable(0, 1);
  HALSIM_SetJoystickPOVsAvailable(0, 1);
  uint8_t fingerCounts[HAL_MAX_JOYSTICK_TOUCHPADS] = {1, 0};
  HALSIM_SetJoystickTouchpadCounts(0, 1, fingerCounts);
  wpi::sim::DriverStationSim::NotifyNewData();

  StderrCapture capture;

  wpi::internal::DriverStationBackend::GetStickButton(0, 1);
  wpi::internal::DriverStationBackend::GetStickButton(0, 2);
  wpi::internal::DriverStationBackend::GetStickAxis(0, 1);
  wpi::internal::DriverStationBackend::GetStickPOV(0, 1);
  wpi::internal::DriverStationBackend::GetStickTouchpadFinger(0, 0, 1);

  CHECK(IsDriverStationAlertActive("joystick0ButtonUnavailable",
                                   "Joystick Button 2 on port 0 not available",
                                   wpi::util::Alert::Level::MEDIUM));
  CHECK(IsDriverStationAlertActive("joystick0AxisUnavailable",
                                   "Joystick axis 1 on port 0 not available",
                                   wpi::util::Alert::Level::MEDIUM));
  CHECK(IsDriverStationAlertActive("joystick0POVUnavailable",
                                   "Joystick POV 1 on port 0 not available",
                                   wpi::util::Alert::Level::MEDIUM));
  CHECK(IsDriverStationAlertActive(
      "joystick0TouchpadFingerUnavailable",
      "Joystick touchpad finger 1 on touchpad 0 on port 0 not available",
      wpi::util::Alert::Level::MEDIUM));
  CHECK_FALSE(IsJoystickDisconnectedAlertActive(0));
  CHECK(capture.Stop() == "");

  ResetJoystickAlerts();
}

TEST_CASE(
    "DriverStation joystick alert collision does not commit partial "
    "alerts",
    "[wpilibc][driverstation]") {
  ResetJoystickAlerts();

  HALSIM_SetJoystickButtonsAvailable(0, 1);
  HALSIM_SetJoystickAxesAvailable(0, 1);
  HALSIM_SetJoystickPOVsAvailable(0, 1);
  wpi::sim::DriverStationSim::NotifyNewData();

  {
    wpi::util::Alert collision{"DriverStation", "joystick0AxisUnavailable",
                               "collision", wpi::util::Alert::Level::MEDIUM};
    REQUIRE(collision);

    StderrCapture capture;

    CHECK_FALSE(wpi::internal::DriverStationBackend::GetStickButton(0, 2));
    CHECK(capture.Stop() == "");

    auto alerts = wpi::sim::AlertSim::GetAll();
    REQUIRE(alerts.size() == 1u);
    CHECK(alerts[0].group == "DriverStation");
    CHECK(alerts[0].id == "joystick0AxisUnavailable");
    CHECK(alerts[0].text == "collision");
    CHECK(alerts[0].level == wpi::util::Alert::Level::MEDIUM);
    CHECK_FALSE(alerts[0].isActive());

    CHECK(wpi::internal::DriverStationBackend::GetStickAxis(0, 1) == 0.0);
    CHECK(wpi::sim::AlertSim::GetAll().size() == 1u);
  }

  CHECK_FALSE(wpi::internal::DriverStationBackend::GetStickButton(0, 2));
  CHECK(IsDriverStationAlertActive("joystick0ButtonUnavailable",
                                   "Joystick Button 2 on port 0 not available",
                                   wpi::util::Alert::Level::MEDIUM));

  ResetJoystickAlerts();
}
