// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/commands2/CommandScheduler.hpp"
#include "wpi/commands2/button/Trigger.hpp"
#include "wpi/driverstation/GenericHID.hpp"

namespace wpi {
class EventLoop;
}  // namespace wpi

namespace wpi::cmd {

/** Minimal base class for generated command controller wrappers. */
class CommandHIDBase {
 protected:
  CommandHIDBase() = default;

  Trigger Button(const wpi::GenericHID& hid, int button,
                 wpi::EventLoop* loop = CommandScheduler::GetInstance()
                                            .GetDefaultButtonLoop()) const;

  Trigger AxisGreaterThan(
      const wpi::GenericHID& hid, int axis, double threshold,
      wpi::EventLoop* loop =
          CommandScheduler::GetInstance().GetDefaultButtonLoop()) const;
};
}  // namespace wpi::cmd
