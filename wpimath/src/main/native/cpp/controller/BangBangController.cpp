// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "../../include/frc/controller/BangBangController.h"

#include <wpi/sendable/SendableBuilder.h>

#include "wpimath/MathShared.h"

using namespace frc;

BangBangController::BangBangController(double tolerance)
    : m_tolerance(tolerance) {
  static int instances = 0;
  instances++;
}

void BangBangController::SetSetpoint(double setpoint) {
  m_setpoint = setpoint;
}

double BangBangController::GetSetpoint() const {
  return m_setpoint;
}

bool BangBangController::AtSetpoint() const {
  return std::abs(m_setpoint - m_measurement) < m_tolerance;
}

void BangBangController::SetTolerance(double tolerance) {
  m_tolerance = tolerance;
}

double BangBangController::GetTolerance() const {
  return m_tolerance;
}

double BangBangController::GetMeasurement() const {
  return m_measurement;
}

double BangBangController::GetError() const {
  return m_setpoint - m_measurement;
}

double BangBangController::Calculate(double measurement, double setpoint) {
  m_measurement = measurement;
  m_setpoint = setpoint;

  return measurement < setpoint ? 1 : 0;
}

double BangBangController::Calculate(double measurement) {
  return Calculate(measurement, m_setpoint);
}

void BangBangController::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("BangBangController");
  builder.AddDoubleProperty(
      "tolerance", [this] { return GetTolerance(); },
      [this](double tolerance) { SetTolerance(tolerance); });
  builder.AddDoubleProperty(
      "setpoint", [this] { return GetSetpoint(); },
      [this](double setpoint) { SetSetpoint(setpoint); });
  builder.AddDoubleProperty(
      "measurement", [this] { return GetMeasurement(); }, nullptr);
  builder.AddDoubleProperty(
      "error", [this] { return GetError(); }, nullptr);
  builder.AddBooleanProperty(
      "atSetpoint", [this] { return AtSetpoint(); }, nullptr);
}
