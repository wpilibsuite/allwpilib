/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include "MotorModel/MotorModelSimulation.h"

namespace frc {
namespace sim {
namespace lowfi {

class MotorSimulator
{
public:
	virtual void SetMotorModelSimulation(const std::shared_ptr<MotorModelSimulation>& motorModelSimulator) = 0;

	virtual double GetPosition() = 0;
	virtual double GetVelocity() = 0;
	virtual double GetAcceleration() = 0;
};

}  // namespace lowfi
}  // namespace sim
}  // namespace frc
