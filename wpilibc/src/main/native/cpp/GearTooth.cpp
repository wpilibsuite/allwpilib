/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/GearTooth.h"

#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc;

constexpr double GearTooth::kGearToothThreshold;

GearTooth::GearTooth(int channel, bool directionSensitive) : Counter(channel) {
  EnableDirectionSensing(directionSensitive);
  SetName("GearTooth", channel);
}

GearTooth::GearTooth(DigitalSource* source, bool directionSensitive)
    : Counter(source) {
  EnableDirectionSensing(directionSensitive);
  SetName("GearTooth", source->GetChannel());
}

GearTooth::GearTooth(std::shared_ptr<DigitalSource> source,
                     bool directionSensitive)
    : Counter(source) {
  EnableDirectionSensing(directionSensitive);
  SetName("GearTooth", source->GetChannel());
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
