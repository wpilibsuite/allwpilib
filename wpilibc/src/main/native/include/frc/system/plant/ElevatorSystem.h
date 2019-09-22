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
 * Constructs the state-space model for an elevator.
 *
 * States: [[position], [velocity]]
 * Inputs: [[voltage]]
 * Outputs: [[position]]
 *
 * @param motor Instance of DCMotor.
 * @param m Carriage mass.
 * @param r Pulley radius.
 * @param G Gear ratio from motor to carriage.
 */
LinearSystem<2, 1, 1> ElevatorSystem(DCMotor motor, units::kilogram_t m,
                                     units::meter_t r, double G);

}  // namespace frc
