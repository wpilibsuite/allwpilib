/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "BuiltInAccelerometer.h"
#include "HAL/HAL.hpp"

/**
 * Constructor.
 * @param range The range the accelerometer will measure
 */
BuiltInAccelerometer::BuiltInAccelerometer(Range range)
	: m_table(0)
{
	setAccelerometerActive(false);
	setAccelerometerRange((AccelerometerRange)range);
	setAccelerometerActive(true);

	HALReport(HALUsageReporting::kResourceType_Accelerometer, 0, 0, "Built-in accelerometer");
}

/**
 * @return The acceleration of the RoboRIO along the X axis in g-forces
 */
double BuiltInAccelerometer::GetX() const
{
	return getAccelerometerX();
}

/**
 * @return The acceleration of the RoboRIO along the Y axis in g-forces
 */
double BuiltInAccelerometer::GetY() const
{
	return getAccelerometerY();
}

/**
 * @return The acceleration of the RoboRIO along the Z axis in g-forces
 */
double BuiltInAccelerometer::GetZ() const
{
	return getAccelerometerZ();
}

std::string BuiltInAccelerometer::GetSmartDashboardType() {
	return "Accelerometer";
}

void BuiltInAccelerometer::InitTable(ITable *subtable) {
	m_table = subtable;
	UpdateTable();
}

void BuiltInAccelerometer::UpdateTable() {
	if (m_table != NULL) {
		m_table->PutNumber("X", GetX());
		m_table->PutNumber("Y", GetY());
		m_table->PutNumber("Z", GetZ());
	}
}

ITable* BuiltInAccelerometer::GetTable() {
	return m_table;
}
