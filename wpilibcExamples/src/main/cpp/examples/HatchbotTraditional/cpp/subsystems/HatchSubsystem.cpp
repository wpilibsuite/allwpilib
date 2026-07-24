// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/HatchSubsystem.hpp"

#include "wpi/telemetry/TelemetryTable.hpp"

using namespace HatchConstants;

HatchSubsystem::HatchSubsystem()
    : hatchSolenoid{0, wpi::PneumaticsModuleType::CTRE_PCM,
                    kHatchSolenoidPorts[0], kHatchSolenoidPorts[1]} {}

void HatchSubsystem::GrabHatch() {
  hatchSolenoid.Set(wpi::DoubleSolenoid::FORWARD);
}

void HatchSubsystem::ReleaseHatch() {
  hatchSolenoid.Set(wpi::DoubleSolenoid::REVERSE);
}

void HatchSubsystem::LogTo(wpi::TelemetryTable& table) const {
  SubsystemBase::LogTo(table);

  // Publish the solenoid state to telemetry.
  table.Log("extended", hatchSolenoid.Get() == wpi::DoubleSolenoid::FORWARD);
}
