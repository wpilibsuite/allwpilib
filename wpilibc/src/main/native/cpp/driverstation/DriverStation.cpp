// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/DriverStation.hpp"

#include <array>
#include <memory>
#include <mutex>

#include "wpi/driverstation/Gamepad.hpp"
#include "wpi/driverstation/GenericHID.hpp"
#include "wpi/system/Errors.hpp"

using namespace wpi;

namespace {
std::mutex dsMutex;
std::array<std::unique_ptr<GenericHID>,
           wpi::internal::DriverStationBackend::JOYSTICK_PORTS>
    hids;
std::array<std::unique_ptr<Gamepad>,
           wpi::internal::DriverStationBackend::JOYSTICK_PORTS>
    gamepads;

void ValidatePort(int port) {
  if (port < 0 || port >= wpi::internal::DriverStationBackend::JOYSTICK_PORTS) {
    throw WPILIB_MakeError(warn::BadJoystickIndex, "port {} out of range",
                           port);
  }
}
}  // namespace

GenericHID& DriverStation::GetGenericHIDUnderLock(int port) {
  ValidatePort(port);

  if (!hids[port]) {
    hids[port].reset(new GenericHID(port));
  }

  return *hids[port];
}

GenericHID& DriverStation::GetGenericHID(int port) {
  std::scoped_lock lock{dsMutex};
  return GetGenericHIDUnderLock(port);
}

// @Common - This is one of the commonly used methods for this class
Gamepad& DriverStation::GetGamepad(int port) {
  std::scoped_lock lock{dsMutex};
  ValidatePort(port);

  if (!gamepads[port]) {
    gamepads[port] = std::make_unique<Gamepad>(GetGenericHIDUnderLock(port));
  }

  return *gamepads[port];
}

void wpi::internal::DriverStationBackend::ResetCachedHIDData() {
  std::scoped_lock lock{dsMutex};

  for (auto& gamepad : gamepads) {
    gamepad.reset();
  }
  for (auto& hid : hids) {
    hid.reset();
  }
}
