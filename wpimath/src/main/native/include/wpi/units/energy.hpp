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
 * @namespace units::energy
 * @brief namespace for unit types and containers representing energy values
 * @details The SI unit for energy is `joules`, and the corresponding
 *          `base_unit` category is `energy_unit`.
 * @anchor energyContainers
 * @sa See unit_t for more information on unit type containers.
 */
#if !defined(DISABLE_PREDEFINED_UNITS) || \
    defined(ENABLE_PREDEFINED_ENERGY_UNITS)
UNIT_ADD_WITH_METRIC_PREFIXES(energy, joule, joules, J,
                              unit<std::ratio<1>, units::category::energy_unit>)
UNIT_ADD_WITH_METRIC_PREFIXES(energy, calorie, calories, cal,
                              unit<std::ratio<4184, 1000>, joules>)
UNIT_ADD(energy, kilowatt_hour, kilowatt_hours, kWh,
         unit<std::ratio<36, 10>, megajoules>)
UNIT_ADD(energy, watt_hour, watt_hours, Wh,
         unit<std::ratio<1, 1000>, kilowatt_hours>)
UNIT_ADD(energy, british_thermal_unit, british_thermal_units, BTU,
         unit<std::ratio<105505585262, 100000000>, joules>)
UNIT_ADD(energy, british_thermal_unit_iso, british_thermal_units_iso, BTU_iso,
         unit<std::ratio<1055056, 1000>, joules>)
UNIT_ADD(energy, british_thermal_unit_59, british_thermal_units_59, BTU59,
         unit<std::ratio<1054804, 1000>, joules>)
UNIT_ADD(energy, therm, therms, thm,
         unit<std::ratio<100000>, british_thermal_units_59>)
UNIT_ADD(energy, foot_pound, foot_pounds, ftlbf,
         unit<std::ratio<13558179483314004, 10000000000000000>, joules>)

UNIT_ADD_CATEGORY_TRAIT(energy)
#endif

using namespace energy;
}  // namespace units
