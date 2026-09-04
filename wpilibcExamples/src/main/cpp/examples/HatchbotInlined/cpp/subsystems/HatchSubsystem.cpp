// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/HatchSubsystem.hpp"

#include "wpi/telemetry/TelemetryTable.hpp"

using namespace HatchConstants;

HatchSubsystem::HatchSubsystem()
    : hatchSolenoid{wpi::CANPort::CAN_S0, wpi::PneumaticsModuleType::CTRE_PCM,
                    HATCH_SOLENOID_PORTS[0], HATCH_SOLENOID_PORTS[1]} {}

wpi::cmd::CommandPtr HatchSubsystem::GrabHatchCommand() {
  // implicitly require `this`
  return this->RunOnce(
      [this] { hatchSolenoid.Set(wpi::DoubleSolenoid::FORWARD); });
}

wpi::cmd::CommandPtr HatchSubsystem::ReleaseHatchCommand() {
  // implicitly require `this`
  return this->RunOnce(
      [this] { hatchSolenoid.Set(wpi::DoubleSolenoid::REVERSE); });
}

void HatchSubsystem::LogTo(wpi::telemetry::TelemetryTable& table) const {
  SubsystemBase::LogTo(table);

  // Publish the solenoid state to telemetry.
  table.Log("extended", hatchSolenoid.Get() == wpi::DoubleSolenoid::FORWARD);
}
