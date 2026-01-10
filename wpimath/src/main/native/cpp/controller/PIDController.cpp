// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/controller/PIDController.hpp"

#include "wpi/telemetry/TelemetryTable.hpp"

using namespace wpi::math;

void PIDController::LogTo(wpi::TelemetryTable& table) const {
  table.Log("p", GetP());
  table.Log("i", GetI());
  table.Log("d", GetD());
  table.Log("izone", GetIZone());
  table.Log("setpoint", GetSetpoint());
  table.Log("measurement", m_measurement);
  table.Log("error", GetError());
  table.Log("error derivative", GetErrorDerivative());
  table.Log("previous error", m_prevError);
  table.Log("total error", GetAccumulatedError());
}

std::string_view PIDController::GetTelemetryType() const {
  return "PIDController";
}
