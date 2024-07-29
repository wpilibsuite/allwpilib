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

namespace units {
/**
 * @namespace units::solid_angle
 * @brief namespace for unit types and containers representing solid_angle
 *        values
 * @details The SI unit for solid_angle is `steradians`, and the corresponding
 *          `base_unit` category is `solid_angle_unit`.
 * @anchor solidAngleContainers
 * @sa See unit_t for more information on unit type containers.
 */
#if !defined(DISABLE_PREDEFINED_UNITS) || \
    defined(ENABLE_PREDEFINED_SOLID_ANGLE_UNITS)
UNIT_ADD_WITH_METRIC_PREFIXES(
    solid_angle, steradian, steradians, sr,
    unit<std::ratio<1>, units::category::solid_angle_unit>)
UNIT_ADD(solid_angle, degree_squared, degrees_squared, sq_deg,
         squared<angle::degrees>)
UNIT_ADD(solid_angle, spat, spats, sp,
         unit<std::ratio<4>, steradians, std::ratio<1>>)

UNIT_ADD_CATEGORY_TRAIT(solid_angle)
#endif

using namespace solid_angle;
}  // namespace units
