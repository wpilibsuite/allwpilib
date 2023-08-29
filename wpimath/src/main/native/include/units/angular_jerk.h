// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "units/angle.h"
#include "units/base.h"
#include "units/time.h"

namespace units {
/**
 * @namespace units::angular_jerk
 * @brief namespace for unit types and containers representing angular
 *        jerk values
 * @details The SI unit for angular jerk is
 *          `radians_per_second_cubed`, and the corresponding `base_unit`
 *          category is`angular_jerk_unit`.
 * @anchor angularJerkContainers
 * @sa See unit_t for more information on unit type containers.
 */
UNIT_ADD(angular_jerk, radians_per_second_cubed, radians_per_second_cubed,
         rad_per_s_cu, unit<std::ratio<1>, units::category::angular_jerk_unit>)
UNIT_ADD(angular_jerk, degrees_per_second_cubed, degrees_per_second_cubed,
         deg_per_s_cu,
         compound_unit<angle::degrees, inverse<cubed<time::seconds>>>)
UNIT_ADD(angular_jerk, turns_per_second_cubed, turns_per_second_cubed,
         tr_per_s_cu,
         compound_unit<angle::turns, inverse<cubed<time::seconds>>>)

UNIT_ADD_CATEGORY_TRAIT(angular_jerk)

using namespace angular_jerk;
}  // namespace units
