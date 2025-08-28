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
#include "units/length.h"

namespace units {
/**
 * @namespace units::volume
 * @brief namespace for unit types and containers representing volume values
 * @details The SI unit for volume is `cubic_meters`, and the corresponding
 *          `base_unit` category is `volume_unit`.
 * @anchor volumeContainers
 * @sa See unit_t for more information on unit type containers.
 */
#if !defined(DISABLE_PREDEFINED_UNITS) || \
    defined(ENABLE_PREDEFINED_VOLUME_UNITS)
UNIT_ADD(volume, cubic_meter, cubic_meters, cu_m,
         unit<std::ratio<1>, units::category::volume_unit>)
UNIT_ADD(volume, cubic_millimeter, cubic_millimeters, cu_mm,
         cubed<length::millimeter>)
UNIT_ADD(volume, cubic_kilometer, cubic_kilometers, cu_km,
         cubed<length::kilometer>)
UNIT_ADD_WITH_METRIC_PREFIXES(volume, liter, liters, L,
                              cubed<deci<length::meter>>)
UNIT_ADD(volume, cubic_inch, cubic_inches, cu_in, cubed<length::inches>)
UNIT_ADD(volume, cubic_foot, cubic_feet, cu_ft, cubed<length::feet>)
UNIT_ADD(volume, cubic_yard, cubic_yards, cu_yd, cubed<length::yards>)
UNIT_ADD(volume, cubic_mile, cubic_miles, cu_mi, cubed<length::miles>)
UNIT_ADD(volume, gallon, gallons, gal, unit<std::ratio<231>, cubic_inches>)
UNIT_ADD(volume, quart, quarts, qt, unit<std::ratio<1, 4>, gallons>)
UNIT_ADD(volume, pint, pints, pt, unit<std::ratio<1, 2>, quarts>)
UNIT_ADD(volume, cup, cups, c, unit<std::ratio<1, 2>, pints>)
UNIT_ADD(volume, fluid_ounce, fluid_ounces, fl_oz, unit<std::ratio<1, 8>, cups>)
UNIT_ADD(volume, barrel, barrels, bl, unit<std::ratio<42>, gallons>)
UNIT_ADD(volume, bushel, bushels, bu,
         unit<std::ratio<215042, 100>, cubic_inches>)
UNIT_ADD(volume, cord, cords, cord, unit<std::ratio<128>, cubic_feet>)
UNIT_ADD(volume, cubic_fathom, cubic_fathoms, cu_fm, cubed<length::fathom>)
UNIT_ADD(volume, tablespoon, tablespoons, tbsp,
         unit<std::ratio<1, 2>, fluid_ounces>)
UNIT_ADD(volume, teaspoon, teaspoons, tsp, unit<std::ratio<1, 6>, fluid_ounces>)
UNIT_ADD(volume, pinch, pinches, pinch, unit<std::ratio<1, 8>, teaspoons>)
UNIT_ADD(volume, dash, dashes, dash, unit<std::ratio<1, 2>, pinches>)
UNIT_ADD(volume, drop, drops, drop, unit<std::ratio<1, 360>, fluid_ounces>)
UNIT_ADD(volume, fifth, fifths, fifth, unit<std::ratio<1, 5>, gallons>)
UNIT_ADD(volume, dram, drams, dr, unit<std::ratio<1, 8>, fluid_ounces>)
UNIT_ADD(volume, gill, gills, gi, unit<std::ratio<4>, fluid_ounces>)
UNIT_ADD(volume, peck, pecks, pk, unit<std::ratio<1, 4>, bushels>)
UNIT_ADD(volume, sack, sacks, sacks, unit<std::ratio<3>, bushels>)
UNIT_ADD(volume, shot, shots, shots, unit<std::ratio<3, 2>, fluid_ounces>)
UNIT_ADD(volume, strike, strikes, strikes, unit<std::ratio<2>, bushels>)

UNIT_ADD_CATEGORY_TRAIT(volume)
#endif

using namespace volume;
}  // namespace units
