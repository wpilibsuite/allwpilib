//--------------------------------------------------------------------------------------------------
//
//	UnitConversion: A compile-time c++14 unit conversion library with no dependencies
//
//--------------------------------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
// BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//--------------------------------------------------------------------------------------------------
//
// Copyright (c) 2016 Nic Holthaus
//
//--------------------------------------------------------------------------------------------------
//
// ATTRIBUTION:
// Parts of this work have been adapted from:
// http://stackoverflow.com/questions/35069778/create-comparison-trait-for-template-classes-whose-parameters-are-in-a-different
// http://stackoverflow.com/questions/28253399/check-traits-for-all-variadic-template-arguments/28253503
// http://stackoverflow.com/questions/36321295/rational-approximation-of-square-root-of-stdratio-at-compile-time?noredirect=1#comment60266601_36321295
// https://github.com/swatanabe/cppnow17-units
//
//--------------------------------------------------------------------------------------------------
//
/// @file	units/energy.h
/// @brief	units representing energy values
//
//--------------------------------------------------------------------------------------------------

#pragma once

#ifndef units_energy_h_
#define units_energy_h_

#include <units/core.h>

namespace units
{
	/**
	 * @namespace	units::energy
	 * @brief		namespace for unit types and containers representing energy values
	 * @details		The SI unit for energy is `joules`, and the corresponding `dimension` dimension is
	 *				`energy_unit`.
	 * @anchor		energyContainers
	 * @sa			See unit for more information on unit type containers.
	 */
	UNIT_ADD_WITH_METRIC_PREFIXES(energy, joules, J, conversion_factor<std::ratio<1>, dimension::energy>)
	UNIT_ADD_WITH_METRIC_PREFIXES(energy, calories, cal, conversion_factor<std::ratio<4184, 1000>, joules_>)
	UNIT_ADD(energy, kilowatt_hours, kWh, conversion_factor<std::ratio<36, 10>, megajoules_>)
	UNIT_ADD(energy, watt_hours, Wh, conversion_factor<std::ratio<1, 1000>, kilowatt_hours_>)
	UNIT_ADD(energy, british_thermal_units, BTU, conversion_factor<std::ratio<105505585262, 100000000>, joules_>)
	UNIT_ADD(energy, british_thermal_units_iso, BTU_iso, conversion_factor<std::ratio<1055056, 1000>, joules_>)
	UNIT_ADD(energy, british_thermal_units_59, BTU59, conversion_factor<std::ratio<1054804, 1000>, joules_>)
	UNIT_ADD(energy, therms, thm, conversion_factor<std::ratio<100000>, british_thermal_units_59_>)
	UNIT_ADD(energy, foot_pounds, ftlbf, conversion_factor<std::ratio<13558179483314004, 10000000000000000>, joules_>)

	UNIT_ADD_DIMENSION_TRAIT(energy)
} // namespace units

#endif // units_energy_h_