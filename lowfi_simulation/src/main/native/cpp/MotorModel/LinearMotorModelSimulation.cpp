/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "MotorModel/LinearMotorModelSimulation.h"

namespace frc {
namespace sim {
namespace lowfi {

const double LinearMotorModelSimulation::MAX_EXPECTED_VOLTAGE = 12.0;


LinearMotorModelSimulation::LinearMotorModelSimulation(double maxSpeed) :
		m_voltagePercentage(0),
		m_maxSpeed(maxSpeed),
		m_position(0),
		m_velocity(0)
{
}

LinearMotorModelSimulation::~LinearMotorModelSimulation()
{
}

void LinearMotorModelSimulation::Reset()
{
	m_position = 0;
	m_velocity = 0;
}

void LinearMotorModelSimulation::SetVoltage(double voltage)
{
	m_voltagePercentage = voltage / MAX_EXPECTED_VOLTAGE;
}

void LinearMotorModelSimulation::Update(double elapsedTime)
{
	m_velocity = m_maxSpeed * m_voltagePercentage;
	m_position += m_velocity * elapsedTime;
}

double LinearMotorModelSimulation::GetPosition()
{
	return m_position;
}

double LinearMotorModelSimulation::GetVelocity()
{
	return m_velocity;
}

double LinearMotorModelSimulation::GetAcceleration()
{
	return 0;
}

}  // namespace lowfi
}  // namespace sim
}  // namespace frc
