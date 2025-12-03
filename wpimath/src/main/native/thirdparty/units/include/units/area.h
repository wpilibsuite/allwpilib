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
/// @file	units/area.h
/// @brief	units representing area values
//
//--------------------------------------------------------------------------------------------------

#pragma once

#ifndef units_area_h_
#define units_area_h_

#include <units/length.h>

namespace units
{
	/**
	 * @namespace	units::area
	 * @brief		namespace for unit types and containers representing area values
	 * @details		The SI unit for area is `square_meters`, and the corresponding `dimension` dimension is
	 *				`area_unit`.
	 * @anchor		areaContainers
	 * @sa			See unit for more information on unit type containers.
	 */
	UNIT_ADD(area, square_meters, m2, conversion_factor<std::ratio<1>, dimension::area>)
	UNIT_ADD(area, square_feet, ft2, squared<feet_>)
	UNIT_ADD(area, square_inches, in2, squared<inches_>)
	UNIT_ADD(area, square_miles, mi2, squared<miles_>)
	UNIT_ADD(area, square_kilometers, km2, squared<kilometers_>)
	UNIT_ADD(area, hectares, ha, conversion_factor<std::ratio<10000>, square_meters_>)
	UNIT_ADD(area, acres, acre, conversion_factor<std::ratio<43560>, square_feet_>)

	UNIT_ADD_DIMENSION_TRAIT(area)
} // namespace units

#endif // units_area_h_