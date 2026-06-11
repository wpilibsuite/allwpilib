// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/button/CommandHIDBase.hpp"

using namespace wpi::cmd;

Trigger CommandHIDBase::Button(const wpi::GenericHID& hid, int button,
                               wpi::EventLoop* loop) const {
  const auto* hidPtr = &hid;
  return Trigger(loop,
                 [hidPtr, button] { return hidPtr->GetRawButton(button); });
}

Trigger CommandHIDBase::AxisGreaterThan(const wpi::GenericHID& hid, int axis,
                                        double threshold,
                                        wpi::EventLoop* loop) const {
  const auto* hidPtr = &hid;
  return Trigger(loop, [hidPtr, axis, threshold] {
    return hidPtr->GetRawAxis(axis) > threshold;
  });
}
