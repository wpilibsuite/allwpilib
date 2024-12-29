// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/controller/PIDController.h"

#include <wpi/sendable/SendableBuilder.h>

using namespace frc;

void PIDController::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("PIDController");
  builder.AddDoubleProperty(
      "p", [this] { return GetP(); }, [this](double value) { SetP(value); });
  builder.AddDoubleProperty(
      "i", [this] { return GetI(); }, [this](double value) { SetI(value); });
  builder.AddDoubleProperty(
      "d", [this] { return GetD(); }, [this](double value) { SetD(value); });
  builder.AddDoubleProperty(
      "izone", [this] { return GetIZone(); },
      [this](double value) { SetIZone(value); });
  builder.AddDoubleProperty(
      "setpoint", [this] { return GetSetpoint(); },
      [this](double value) { SetSetpoint(value); });
  builder.AddDoubleProperty(
      "measurement", [this] { return m_measurement; }, nullptr);
  builder.AddDoubleProperty("error", [this] { return GetError(); }, nullptr);
  builder.AddDoubleProperty(
      "error derivative", [this] { return GetErrorDerivative(); }, nullptr);
  builder.AddDoubleProperty(
      "previous error", [this] { return m_prevError; }, nullptr);
  builder.AddDoubleProperty(
      "total error", [this] { return GetAccumulatedError(); }, nullptr);
}
