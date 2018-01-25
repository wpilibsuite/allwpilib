/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Pneumatics.h"

#include <SmartDashboard/SmartDashboard.h>

Pneumatics::Pneumatics()
    : frc::Subsystem("Pneumatics") {
	AddChild("Pressure Sensor", m_pressureSensor);
}

/**
 * No default command
 */
void Pneumatics::InitDefaultCommand() {}

/**
 * Start the compressor going. The compressor automatically starts and stops as
 * it goes above and below maximum pressure.
 */
void Pneumatics::Start() {
#ifndef SIMULATION
	m_compressor.Start();
#endif
}

/**
 * @return Whether or not the system is fully pressurized.
 */
bool Pneumatics::IsPressurized() {
#ifndef SIMULATION
	return kMaxPressure <= m_pressureSensor.GetVoltage();
#else
	return true;  // NOTE: Simulation always has full pressure
#endif
}

/**
 * Puts the pressure on the SmartDashboard.
 */
void Pneumatics::WritePressure() {
	frc::SmartDashboard::PutNumber(
			"Pressure", m_pressureSensor.GetVoltage());
}
