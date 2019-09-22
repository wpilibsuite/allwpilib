/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <units/units.h>

#include "frc/system/LinearSystem.h"
#include "frc/system/plant/DCMotor.h"

namespace frc {

/**
 * Constructs the state-space model for a single-jointed arm.
 *
 * States: [[angle], [angular velocity]]
 * Inputs: [[voltage]]
 * Outputs: [[angle]]
 *
 * @param motor Instance of DCMotor.
 * @param J Moment of inertia.
 * @param G Gear ratio from motor to carriage.
 */
LinearSystem<2, 1, 1> SingleJointedArmSystem(DCMotor motor,
                                             units::kilogram_square_meter_t J,
                                             double G);

}  // namespace frc
