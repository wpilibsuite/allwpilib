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
//
//--------------------------------------------------------------------------------------------------
//
// Copyright (c) 2016 Nic Holthaus
//
//--------------------------------------------------------------------------------------------------

#pragma once

#include "units/base.h"
#include "units/energy.h"
#include "units/frequency.h"
#include "units/mass.h"

namespace units {
/**
 * @namespace units::radiation
 * @brief namespace for unit types and containers representing radiation values
 * @details The SI units for radiation are:
 *          - source activity:	becquerel
 *          - absorbed dose:	gray
 *          - equivalent dose:	sievert
 * @anchor radiationContainers
 * @sa See unit_t for more information on unit type containers.
 */
#if !defined(DISABLE_PREDEFINED_UNITS) || \
    defined(ENABLE_PREDEFINED_RADIATION_UNITS)
UNIT_ADD_WITH_METRIC_PREFIXES(radiation, becquerel, becquerels, Bq,
                              unit<std::ratio<1>, units::frequency::hertz>)
UNIT_ADD_WITH_METRIC_PREFIXES(
    radiation, gray, grays, Gy,
    compound_unit<energy::joules, inverse<mass::kilogram>>)
UNIT_ADD_WITH_METRIC_PREFIXES(radiation, sievert, sieverts, Sv,
                              unit<std::ratio<1>, grays>)
UNIT_ADD(radiation, curie, curies, Ci, unit<std::ratio<37>, gigabecquerels>)
UNIT_ADD(radiation, rutherford, rutherfords, rd,
         unit<std::ratio<1>, megabecquerels>)
UNIT_ADD(radiation, rad, rads, rads, unit<std::ratio<1>, centigrays>)

UNIT_ADD_CATEGORY_TRAIT(radioactivity)
#endif

using namespace radiation;
}  // namespace units
