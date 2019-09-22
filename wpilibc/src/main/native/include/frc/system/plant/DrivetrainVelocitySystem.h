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
 * Constructs the state-space model for a drivetrain.
 *
 * States: [[left velocity], [right velocity]]
 * Inputs: [[left voltage], [right voltage]]
 * Outputs: [[left velocity], [right velocity]]
 *
 * @param motor Instance of DCMotor.
 * @param m Drivetrain mass.
 * @param r Wheel radius.
 * @param rb Robot radius.
 * @param G Gear ratio from motor to wheel.
 * @param J Moment of inertia.
 */
LinearSystem<2, 2, 2> DrivetrainVelocitySystem(
    DCMotor motor, units::kilogram_t m, units::meter_t r, units::meter_t rb,
    double G, units::kilogram_square_meter_t J);

}  // namespace frc
