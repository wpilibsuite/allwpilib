// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

// Copyright (c) 2016 Nic Holthaus
//
// The MIT License (MIT)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "units/angle.h"
#include "units/base.h"
#include "units/time.h"

namespace units {
/**
 * @namespace units::angular_velocity
 * @brief namespace for unit types and containers representing angular velocity
 *        values
 * @details The SI unit for angular velocity is `radians_per_second`, and the
 *          corresponding `base_unit` category is`angular_velocity_unit`.
 * @anchor angularVelocityContainers
 * @sa See unit_t for more information on unit type containers.
 */
#if !defined(DISABLE_PREDEFINED_UNITS) || \
    defined(ENABLE_PREDEFINED_ANGULAR_VELOCITY_UNITS)
UNIT_ADD(angular_velocity, radians_per_second, radians_per_second, rad_per_s,
         unit<std::ratio<1>, units::category::angular_velocity_unit>)
UNIT_ADD(angular_velocity, degrees_per_second, degrees_per_second, deg_per_s,
         compound_unit<angle::degrees, inverse<time::seconds>>)
UNIT_ADD(angular_velocity, revolutions_per_minute, revolutions_per_minute, rpm,
         unit<std::ratio<2, 60>, radians_per_second, std::ratio<1>>)
UNIT_ADD(angular_velocity, milliarcseconds_per_year, milliarcseconds_per_year,
         mas_per_yr, compound_unit<angle::milliarcseconds, inverse<time::year>>)

UNIT_ADD_CATEGORY_TRAIT(angular_velocity)
#endif

using namespace angular_velocity;
}  // namespace units
