// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "units/angular_velocity.h"
#include "units/base.h"
#include "units/time.h"

namespace units {
/**
 * @namespace units::angular_acceleration
 * @brief namespace for unit types and containers representing angular
 *        acceleration values
 * @details The SI unit for angular acceleration is
 *          `radians_per_second_squared`, and the corresponding `base_unit`
 *          category is`angular_acceleration_unit`.
 * @anchor angularAccelerationContainers
 * @sa See unit_t for more information on unit type containers.
 */
UNIT_ADD(angular_acceleration, radians_per_second_squared,
         radians_per_second_squared, rad_per_s_sq,
         unit<std::ratio<1>, units::category::angular_acceleration_unit>)
UNIT_ADD(angular_acceleration, degrees_per_second_squared,
         degrees_per_second_squared, deg_per_s_sq,
         compound_unit<angle::degrees, inverse<squared<time::seconds>>>)

UNIT_ADD_CATEGORY_TRAIT(angular_acceleration)

using namespace angular_acceleration;
}  // namespace units
