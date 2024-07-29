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
#include "units/energy.h"
#include "units/force.h"
#include "units/length.h"

namespace units {
/**
 * @namespace units::torque
 * @brief namespace for unit types and containers representing torque values
 * @details The SI unit for torque is `newton_meters`, and the corresponding
 *          `base_unit` category is `torque_units`.
 * @anchor torqueContainers
 * @sa See unit_t for more information on unit type containers.
 */
#if !defined(DISABLE_PREDEFINED_UNITS) || \
    defined(ENABLE_PREDEFINED_TORQUE_UNITS)
UNIT_ADD(torque, newton_meter, newton_meters, Nm,
         unit<std::ratio<1>, units::energy::joule>)
UNIT_ADD(torque, foot_pound, foot_pounds, ftlb,
         compound_unit<length::foot, force::pounds>)
UNIT_ADD(torque, foot_poundal, foot_poundals, ftpdl,
         compound_unit<length::foot, force::poundal>)
UNIT_ADD(torque, inch_pound, inch_pounds, inlb,
         compound_unit<length::inch, force::pounds>)
UNIT_ADD(torque, meter_kilogram, meter_kilograms, mkgf,
         compound_unit<length::meter, force::kiloponds>)

UNIT_ADD_CATEGORY_TRAIT(torque)
#endif

using namespace torque;
}  // namespace units
