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
#include "units/force.h"
#include "units/length.h"

namespace units {
/**
 * @namespace units::pressure
 * @brief namespace for unit types and containers representing pressure values
 * @details The SI unit for pressure is `pascals`, and the corresponding
 *          `base_unit` category is `pressure_unit`.
 * @anchor pressureContainers
 * @sa See unit_t for more information on unit type containers.
 */
#if !defined(DISABLE_PREDEFINED_UNITS) || \
    defined(ENABLE_PREDEFINED_PRESSURE_UNITS)
UNIT_ADD_WITH_METRIC_PREFIXES(
    pressure, pascal, pascals, Pa,
    unit<std::ratio<1>, units::category::pressure_unit>)
UNIT_ADD(pressure, bar, bars, bar, unit<std::ratio<100>, kilo<pascals>>)
UNIT_ADD(pressure, mbar, mbars, mbar, unit<std::ratio<1>, milli<bar>>)
UNIT_ADD(pressure, atmosphere, atmospheres, atm,
         unit<std::ratio<101325>, pascals>)
UNIT_ADD(pressure, pounds_per_square_inch, pounds_per_square_inch, psi,
         compound_unit<force::pounds, inverse<squared<length::inch>>>)
UNIT_ADD(pressure, torr, torrs, torr, unit<std::ratio<1, 760>, atmospheres>)

UNIT_ADD_CATEGORY_TRAIT(pressure)
#endif

using namespace pressure;
}  // namespace units
