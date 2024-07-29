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
 * @namespace units::time
 * @brief namespace for unit types and containers representing time values
 * @details The SI unit for time is `seconds`, and the corresponding `base_unit`
 *          category is `time_unit`.
 * @anchor timeContainers
 * @sa See unit_t for more information on unit type containers.
 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_TIME_UNITS)
UNIT_ADD_WITH_METRIC_PREFIXES(time, second, seconds, s,
                              unit<std::ratio<1>, units::category::time_unit>)
UNIT_ADD(time, minute, minutes, min, unit<std::ratio<60>, seconds>)
UNIT_ADD(time, hour, hours, hr, unit<std::ratio<60>, minutes>)
UNIT_ADD(time, day, days, d, unit<std::ratio<24>, hours>)
UNIT_ADD(time, week, weeks, wk, unit<std::ratio<7>, days>)
UNIT_ADD(time, year, years, yr, unit<std::ratio<365>, days>)
UNIT_ADD(time, julian_year, julian_years, a_j,
         unit<std::ratio<31557600>, seconds>)
UNIT_ADD(time, gregorian_year, gregorian_years, a_g,
         unit<std::ratio<31556952>, seconds>)

UNIT_ADD_CATEGORY_TRAIT(time)
#endif

using namespace time;
}  // namespace units
