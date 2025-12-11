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
/// @file	units/radiation.h
/// @brief	units representing radiation values
//
//--------------------------------------------------------------------------------------------------

#pragma once

#ifndef units_radiation_h_
#define units_radiation_h_

#include <wpi/units/energy.h>
#include <wpi/units/frequency.h>
#include <wpi/units/mass.h>

namespace wpi::units
{
	/**
	 * @namespace	wpi::units::radiation
	 * @brief		namespace for unit types and containers representing radiation values
	 * @details		The SI units for radiation are:
	 *				- source activity:	becquerel
	 *				- absorbed dose:	gray
	 *				- equivalent dose:	sievert
	 * @anchor		radiationContainers
	 * @sa			See unit for more information on unit type containers.
	 */
	UNIT_ADD_WITH_METRIC_PREFIXES(radiation, becquerels, Bq, conversion_factor<std::ratio<1>, frequency::hertz_>)
	UNIT_ADD_WITH_METRIC_PREFIXES(radiation, grays, Gy, compound_conversion_factor<joules_, inverse<kilograms_>>)
	UNIT_ADD_WITH_METRIC_PREFIXES(radiation, sieverts, Sv, conversion_factor<std::ratio<1>, grays_>)
	UNIT_ADD(radiation, curies, Ci, conversion_factor<std::ratio<37>, gigabecquerels<>>)
	UNIT_ADD(radiation, rutherfords, rd, conversion_factor<std::ratio<1>, megabecquerels<>>)
	UNIT_ADD(radiation, radiation_absorbed_dose, rads, conversion_factor<std::ratio<1>, centigrays<>>)

	UNIT_ADD_DIMENSION_TRAIT(radioactivity)
} // namespace wpi::units

#endif // units_radiation_h_