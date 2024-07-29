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

#include "units/acceleration.h"
#include "units/base.h"
#include "units/length.h"
#include "units/mass.h"
#include "units/time.h"

namespace units {
/**
 * @namespace units::force
 * @brief namespace for unit types and containers representing force values
 * @details The SI unit for force is `newtons`, and the corresponding
 *          `base_unit` category is `force_unit`.
 * @anchor forceContainers
 * @sa See unit_t for more information on unit type containers.
 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_FORCE_UNITS)
UNIT_ADD_WITH_METRIC_PREFIXES(force, newton, newtons, N,
                              unit<std::ratio<1>, units::category::force_unit>)
UNIT_ADD(
    force, pound, pounds, lbf,
    compound_unit<mass::slug, length::foot, inverse<squared<time::seconds>>>)
UNIT_ADD(force, dyne, dynes, dyn, unit<std::ratio<1, 100000>, newtons>)
UNIT_ADD(force, kilopond, kiloponds, kp,
         compound_unit<acceleration::standard_gravity, mass::kilograms>)
UNIT_ADD(
    force, poundal, poundals, pdl,
    compound_unit<mass::pound, length::foot, inverse<squared<time::seconds>>>)

UNIT_ADD_CATEGORY_TRAIT(force)
#endif

using force::newton_t;
using force::newtons;
}  // namespace units
