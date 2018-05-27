/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "MotorModel/MotorModelSimulation.h"

namespace frc {
namespace sim {
namespace lowfi {

class LinearMotorModelSimulation : public MotorModelSimulation
{
public:
	explicit LinearMotorModelSimulation(double maxSpeed);
	~LinearMotorModelSimulation();

	virtual void Reset();
	virtual void SetVoltage(double voltage);
	virtual void Update(double elapsedTime);

	virtual double GetPosition();
	virtual double GetVelocity();
	virtual double GetAcceleration();

protected:
	double m_voltagePercentage;
	double m_maxSpeed;
	double m_position;
	double m_velocity;

	static const double MAX_EXPECTED_VOLTAGE;
};

}  // namespace lowfi
}  // namespace sim
}  // namespace frc
