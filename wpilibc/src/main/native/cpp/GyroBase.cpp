/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/GyroBase.h"

#include "frc/WPIErrors.h"
#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc;

double GyroBase::PIDGet() {
  switch (GetPIDSourceType()) {
    case PIDSourceType::kRate:
      return GetRate();
    case PIDSourceType::kDisplacement:
      return GetAngle();
    default:
      return 0;
  }
}

void GyroBase::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Gyro");
  builder.AddDoubleProperty("Value", [=]() { return GetAngle(); }, nullptr);
}
