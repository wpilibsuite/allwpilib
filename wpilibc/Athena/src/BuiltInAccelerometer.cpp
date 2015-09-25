/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "BuiltInAccelerometer.h"
#include "HAL/HAL.hpp"
#include "WPIErrors.h"
#include "LiveWindow/LiveWindow.h"

/**
 * Constructor.
 * @param range The range the accelerometer will measure
 */
BuiltInAccelerometer::BuiltInAccelerometer(Range range) {
  SetRange(range);

  HALReport(HALUsageReporting::kResourceType_Accelerometer, 0, 0,
            "Built-in accelerometer");
  LiveWindow::GetInstance()->AddSensor((std::string) "BuiltInAccel", 0, this);
}

/** {@inheritdoc} */
void BuiltInAccelerometer::SetRange(Range range) {
  if (range == kRange_16G) {
    wpi_setWPIErrorWithContext(
        ParameterOutOfRange, "16G range not supported (use k2G, k4G, or k8G)");
  }

  setAccelerometerActive(false);
  setAccelerometerRange((AccelerometerRange)range);
  setAccelerometerActive(true);
}

/**
 * @return The acceleration of the RoboRIO along the X axis in g-forces
 */
double BuiltInAccelerometer::GetX() { return getAccelerometerX(); }

/**
 * @return The acceleration of the RoboRIO along the Y axis in g-forces
 */
double BuiltInAccelerometer::GetY() { return getAccelerometerY(); }

/**
 * @return The acceleration of the RoboRIO along the Z axis in g-forces
 */
double BuiltInAccelerometer::GetZ() { return getAccelerometerZ(); }

std::string BuiltInAccelerometer::GetSmartDashboardType() const {
  return "3AxisAccelerometer";
}

void BuiltInAccelerometer::InitTable(std::shared_ptr<ITable> subtable) {
  m_table = subtable;
  UpdateTable();
}

void BuiltInAccelerometer::UpdateTable() {
  if (m_table != nullptr) {
    m_table->PutNumber("X", GetX());
    m_table->PutNumber("Y", GetY());
    m_table->PutNumber("Z", GetZ());
  }
}

std::shared_ptr<ITable> BuiltInAccelerometer::GetTable() const { return m_table; }
