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
 * @namespace units::mass
 * @brief namespace for unit types and containers representing mass values
 * @details The SI unit for mass is `kilograms`, and the corresponding
 *          `base_unit` category is `mass_unit`.
 * @anchor massContainers
 * @sa See unit_t for more information on unit type containers.
 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_MASS_UNITS)
UNIT_ADD_WITH_METRIC_PREFIXES(
    mass, gram, grams, g, unit<std::ratio<1, 1000>, units::category::mass_unit>)
UNIT_ADD(mass, metric_ton, metric_tons, t, unit<std::ratio<1000>, kilograms>)
UNIT_ADD(mass, pound, pounds, lb,
         unit<std::ratio<45359237, 100000000>, kilograms>)
UNIT_ADD(mass, long_ton, long_tons, ln_t, unit<std::ratio<2240>, pounds>)
UNIT_ADD(mass, short_ton, short_tons, sh_t, unit<std::ratio<2000>, pounds>)
UNIT_ADD(mass, stone, stone, st, unit<std::ratio<14>, pounds>)
UNIT_ADD(mass, ounce, ounces, oz, unit<std::ratio<1, 16>, pounds>)
UNIT_ADD(mass, carat, carats, ct, unit<std::ratio<200>, milligrams>)
UNIT_ADD(mass, slug, slugs, slug,
         unit<std::ratio<145939029, 10000000>, kilograms>)

UNIT_ADD_CATEGORY_TRAIT(mass)
#endif

using namespace mass;
}  // namespace units
