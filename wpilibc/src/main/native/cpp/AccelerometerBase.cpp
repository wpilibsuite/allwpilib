/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/AccelerometerBase.h"

#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc;

void AccelerometerBase::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("3AxisAccelerometer");
  builder.AddDoubleProperty("X", [=]() { return GetX(); }, nullptr);
  builder.AddDoubleProperty("Y", [=]() { return GetY(); }, nullptr);
  builder.AddDoubleProperty("Z", [=]() { return GetZ(); }, nullptr);
}
