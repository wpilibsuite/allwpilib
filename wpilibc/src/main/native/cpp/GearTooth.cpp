// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/GearTooth.h"

#include "frc/smartdashboard/SendableBuilder.h"
#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc;

constexpr double GearTooth::kGearToothThreshold;

GearTooth::GearTooth(int channel, bool directionSensitive) : Counter(channel) {
  EnableDirectionSensing(directionSensitive);
  SendableRegistry::GetInstance().SetName(this, "GearTooth", channel);
}

GearTooth::GearTooth(DigitalSource* source, bool directionSensitive)
    : Counter(source) {
  EnableDirectionSensing(directionSensitive);
  SendableRegistry::GetInstance().SetName(this, "GearTooth",
                                          source->GetChannel());
}

GearTooth::GearTooth(std::shared_ptr<DigitalSource> source,
                     bool directionSensitive)
    : Counter(source) {
  EnableDirectionSensing(directionSensitive);
  SendableRegistry::GetInstance().SetName(this, "GearTooth",
                                          source->GetChannel());
}

void GearTooth::EnableDirectionSensing(bool directionSensitive) {
  if (directionSensitive) {
    SetPulseLengthMode(kGearToothThreshold);
  }
}

void GearTooth::InitSendable(SendableBuilder& builder) {
  Counter::InitSendable(builder);
  builder.SetSmartDashboardType("Gear Tooth");
}
