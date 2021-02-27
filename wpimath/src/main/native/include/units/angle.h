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

#include "units/base.h"

namespace units {
/**
 * @namespace units::angle
 * @brief namespace for unit types and containers representing angle values
 * @details The SI unit for angle is `radians`, and the corresponding
 *          `base_unit` category is`angle_unit`.
 * @anchor angleContainers
 * @sa See unit_t for more information on unit type containers.
 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_ANGLE_UNITS)
UNIT_ADD_WITH_METRIC_PREFIXES(angle, radian, radians, rad,
                              unit<std::ratio<1>, units::category::angle_unit>)
UNIT_ADD(angle, degree, degrees, deg,
         unit<std::ratio<1, 180>, radians, std::ratio<1>>)
UNIT_ADD(angle, arcminute, arcminutes, arcmin, unit<std::ratio<1, 60>, degrees>)
UNIT_ADD(angle, arcsecond, arcseconds, arcsec,
         unit<std::ratio<1, 60>, arcminutes>)
UNIT_ADD(angle, milliarcsecond, milliarcseconds, mas, milli<arcseconds>)
UNIT_ADD(angle, turn, turns, tr, unit<std::ratio<2>, radians, std::ratio<1>>)
UNIT_ADD(angle, gradian, gradians, gon, unit<std::ratio<1, 400>, turns>)

UNIT_ADD_CATEGORY_TRAIT(angle)
#endif

using namespace angle;
}  // namespace units
