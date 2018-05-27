/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include "MotorSimulator.h"
#include "Simulation/PWMSim.h"

namespace frc {
namespace sim {
namespace lowfi {

class WpiMotorSimulator : public MotorSimulator
{
public:
	explicit WpiMotorSimulator(int index);
	~WpiMotorSimulator();

	void SetMotorModelSimulation(const std::shared_ptr<MotorModelSimulation>& motorModelSimulator);

	void Update(double elapsedTime);
	double GetPosition() override;
	double GetVelocity() override;
	double GetAcceleration() override;

protected:
	std::shared_ptr<MotorModelSimulation> m_motorModelSimulation;
	frc::sim::PWMSim m_pwmSimulator;
};

}  // namespace lowfi
}  // namespace sim
}  // namespace frc
