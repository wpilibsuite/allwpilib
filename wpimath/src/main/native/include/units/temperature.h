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
// NOTE: temperature units have special conversion overloads, since they
// require translations and aren't a reversible transform.

/**
 * @namespace units::temperature
 * @brief namespace for unit types and containers representing temperature
 *        values
 * @details The SI unit for temperature is `kelvin`, and the corresponding
 *          `base_unit` category is `temperature_unit`.
 * @anchor temperatureContainers
 * @sa See unit_t for more information on unit type containers.
 */
#if !defined(DISABLE_PREDEFINED_UNITS) || \
    defined(ENABLE_PREDEFINED_TEMPERATURE_UNITS)
UNIT_ADD(temperature, kelvin, kelvin, K,
         unit<std::ratio<1>, units::category::temperature_unit>)
UNIT_ADD(temperature, celsius, celsius, degC,
         unit<std::ratio<1>, kelvin, std::ratio<0>, std::ratio<27315, 100>>)
UNIT_ADD(temperature, fahrenheit, fahrenheit, degF,
         unit<std::ratio<5, 9>, celsius, std::ratio<0>, std::ratio<-160, 9>>)
UNIT_ADD(temperature, reaumur, reaumur, Re, unit<std::ratio<10, 8>, celsius>)
UNIT_ADD(temperature, rankine, rankine, Ra, unit<std::ratio<5, 9>, kelvin>)

UNIT_ADD_CATEGORY_TRAIT(temperature)
#endif

using namespace temperature;
}  // namespace units
