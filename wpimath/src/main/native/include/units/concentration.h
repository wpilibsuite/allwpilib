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
 * @namespace units::concentration
 * @brief namespace for unit types and containers representing concentration
 *        values
 * @details The SI unit for concentration is `parts_per_million`, and the
 *          corresponding `base_unit` category is `scalar_unit`.
 * @anchor concentrationContainers
 * @sa See unit_t for more information on unit type containers.
 */
#if !defined(DISABLE_PREDEFINED_UNITS) || \
    defined(ENABLE_PREDEFINED_CONCENTRATION_UNITS)
UNIT_ADD(concentration, ppm, parts_per_million, ppm,
         unit<std::ratio<1, 1000000>, units::category::scalar_unit>)
UNIT_ADD(concentration, ppb, parts_per_billion, ppb,
         unit<std::ratio<1, 1000>, parts_per_million>)
UNIT_ADD(concentration, ppt, parts_per_trillion, ppt,
         unit<std::ratio<1, 1000>, parts_per_billion>)
UNIT_ADD(concentration, percent, percent, pct,
         unit<std::ratio<1, 100>, units::category::scalar_unit>)

UNIT_ADD_CATEGORY_TRAIT(concentration)
#endif

using namespace concentration;
}  // namespace units
