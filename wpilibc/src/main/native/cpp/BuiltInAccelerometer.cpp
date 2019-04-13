/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/BuiltInAccelerometer.h"

#include <hal/Accelerometer.h>
#include <hal/HAL.h>

#include "frc/WPIErrors.h"
#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc;

BuiltInAccelerometer::BuiltInAccelerometer(Range range) {
  SetRange(range);

  HAL_Report(HALUsageReporting::kResourceType_Accelerometer, 0, 0,
             "Built-in accelerometer");
  SetName("BuiltInAccel", 0);
}

void BuiltInAccelerometer::SetRange(Range range) {
  if (range == kRange_16G) {
    wpi_setWPIErrorWithContext(
        ParameterOutOfRange, "16G range not supported (use k2G, k4G, or k8G)");
  }

  HAL_SetAccelerometerActive(false);
  HAL_SetAccelerometerRange((HAL_AccelerometerRange)range);
  HAL_SetAccelerometerActive(true);
}

double BuiltInAccelerometer::GetX() { return HAL_GetAccelerometerX(); }

double BuiltInAccelerometer::GetY() { return HAL_GetAccelerometerY(); }

double BuiltInAccelerometer::GetZ() { return HAL_GetAccelerometerZ(); }

void BuiltInAccelerometer::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("3AxisAccelerometer");
  builder.AddDoubleProperty("X", [=]() { return GetX(); }, nullptr);
  builder.AddDoubleProperty("Y", [=]() { return GetY(); }, nullptr);
  builder.AddDoubleProperty("Z", [=]() { return GetZ(); }, nullptr);
}
