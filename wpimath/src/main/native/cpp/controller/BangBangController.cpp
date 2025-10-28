// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/controller/BangBangController.hpp"

#include <wpi/util/sendable/SendableBuilder.hpp>

using namespace wpi::math;

void BangBangController::InitSendable(wpi::util::SendableBuilder& builder) {
  builder.SetSmartDashboardType("BangBangController");
  builder.AddDoubleProperty(
      "tolerance", [this] { return GetTolerance(); },
      [this](double tolerance) { SetTolerance(tolerance); });
  builder.AddDoubleProperty(
      "setpoint", [this] { return GetSetpoint(); },
      [this](double setpoint) { SetSetpoint(setpoint); });
  builder.AddDoubleProperty(
      "measurement", [this] { return GetMeasurement(); }, nullptr);
  builder.AddDoubleProperty("error", [this] { return GetError(); }, nullptr);
  builder.AddBooleanProperty(
      "atSetpoint", [this] { return AtSetpoint(); }, nullptr);
}
