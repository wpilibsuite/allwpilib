// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/controller/PIDController.hpp"

#include "wpi/telemetry/TelemetryTable.hpp"
#include "wpi/tunables/TunableConfig.hpp"
#include "wpi/tunables/TunableTable.hpp"

using namespace wpi::math;

void PIDController::LogTo(wpi::telemetry::TelemetryTable& table) const {
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

void PIDController::PublishTunable(wpi::tunables::TunableTable& table) {
  table.Publish("p", m_Kp);
  table.Publish("i", m_Ki);
  table.Publish("d", m_Kd);
  table.Publish("izone", m_iZone);
  auto setpointConfig = wpi::tunables::TunableConfig::AlwaysGet();
  setpointConfig.onTune = [](wpi::tunables::detail::TunableBase&,
                             wpi::tunables::ComplexTunable* self) {
    if (auto controller = static_cast<PIDController*>(self)) {
      controller->SetSetpoint(controller->GetSetpoint());
    }
  };
  setpointConfig.parent = this;
  table.Publish("setpoint", this, &PIDController::m_setpoint, setpointConfig);
}

std::string_view PIDController::GetTunableType() const {
  return "PIDController";
}
