// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/HatchSubsystem.hpp"

#include "wpi/telemetry/TelemetryTable.hpp"
#include "Constants.h"

using namespace HatchConstants;

HatchSubsystem::HatchSubsystem()
    : m_hatchSolenoid{0, wpi::PneumaticsModuleType::CTREPCM,
                      kHatchSolenoidPorts[0], kHatchSolenoidPorts[1]} {}

wpi::cmd::CommandPtr HatchSubsystem::GrabHatchCommand() {
  // implicitly require `this`
  return this->RunOnce(
      [this] { m_hatchSolenoid.Set(wpi::DoubleSolenoid::kForward); });
}

wpi::cmd::CommandPtr HatchSubsystem::ReleaseHatchCommand() {
  // implicitly require `this`
  return this->RunOnce(
      [this] { m_hatchSolenoid.Set(wpi::DoubleSolenoid::kReverse); });
}

void HatchSubsystem::UpdateTelemetry(wpi::TelemetryTable& table) const {
  SubsystemBase::UpdateTelemetry(table);

  // Publish the solenoid state to telemetry.
  table.Log("extended", m_hatchSolenoid.Get() == frc::DoubleSolenoid::kForward);
}
