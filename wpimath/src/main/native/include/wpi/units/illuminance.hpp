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
#include "units/luminous_flux.h"

namespace units {
/**
 * @namespace units::illuminance
 * @brief namespace for unit types and containers representing illuminance
 *        values
 * @details The SI unit for illuminance is `luxes`, and the corresponding
 *          `base_unit` category is `illuminance_unit`.
 * @anchor illuminanceContainers
 * @sa See unit_t for more information on unit type containers.
 */
#if !defined(DISABLE_PREDEFINED_UNITS) || \
    defined(ENABLE_PREDEFINED_ILLUMINANCE_UNITS)
UNIT_ADD_WITH_METRIC_PREFIXES(
    illuminance, lux, luxes, lx,
    unit<std::ratio<1>, units::category::illuminance_unit>)
UNIT_ADD(illuminance, footcandle, footcandles, fc,
         compound_unit<luminous_flux::lumen, inverse<squared<length::foot>>>)
UNIT_ADD(illuminance, lumens_per_square_inch, lumens_per_square_inch,
         lm_per_in_sq,
         compound_unit<luminous_flux::lumen, inverse<squared<length::inch>>>)
UNIT_ADD(
    illuminance, phot, phots, ph,
    compound_unit<luminous_flux::lumens, inverse<squared<length::centimeter>>>)

UNIT_ADD_CATEGORY_TRAIT(illuminance)
#endif

using namespace illuminance;
}  // namespace units
