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
 * @namespace units::area
 * @brief namespace for unit types and containers representing area values
 * @details The SI unit for area is `square_meters`, and the corresponding
 *          `base_unit` category is `area_unit`.
 * @anchor areaContainers
 * @sa See unit_t for more information on unit type containers.
 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_AREA_UNITS)
UNIT_ADD(area, square_meter, square_meters, sq_m,
         unit<std::ratio<1>, units::category::area_unit>)
UNIT_ADD(area, square_foot, square_feet, sq_ft, squared<length::feet>)
UNIT_ADD(area, square_inch, square_inches, sq_in, squared<length::inch>)
UNIT_ADD(area, square_mile, square_miles, sq_mi, squared<length::miles>)
UNIT_ADD(area, square_kilometer, square_kilometers, sq_km,
         squared<length::kilometers>)
UNIT_ADD(area, hectare, hectares, ha, unit<std::ratio<10000>, square_meters>)
UNIT_ADD(area, acre, acres, acre, unit<std::ratio<43560>, square_feet>)

UNIT_ADD_CATEGORY_TRAIT(area)
#endif

using namespace area;
}  // namespace units
