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
/// @file	units/volume.h
/// @brief	units representing volume values
//
//--------------------------------------------------------------------------------------------------

#pragma once

#ifndef units_volume_h_
#define units_volume_h_

#include <units/length.h>

namespace units
{
	/**
	 * @namespace	units::volume
	 * @brief		namespace for unit types and containers representing volume values
	 * @details		The SI unit for volume is `cubic_meters`, and the corresponding `dimension` dimension is
	 *				`volume_unit`.
	 * @anchor		volumeContainers
	 * @sa			See unit for more information on unit type containers.
	 */
	UNIT_ADD(volume, cubic_meters, m3, conversion_factor<std::ratio<1>, dimension::volume>)
	UNIT_ADD(volume, cubic_millimeters, mm3, cubed<millimeters_>)
	UNIT_ADD(volume, cubic_kilometers, km3, cubed<kilometers<>>)
	UNIT_ADD_WITH_METRIC_PREFIXES(volume, liters, L, cubed<decimeters<>>)
	UNIT_ADD(volume, cubic_inches, in3, cubed<inches<>>)
	UNIT_ADD(volume, cubic_feet, ft3, cubed<feet<>>)
	UNIT_ADD(volume, cubic_yards, yd3, cubed<yards<>>)
	UNIT_ADD(volume, cubic_miles, mi3, cubed<miles<>>)
	UNIT_ADD(volume, gallons, gal, conversion_factor<std::ratio<231>, cubic_inches<>>)
	UNIT_ADD(volume, quarts, qt, conversion_factor<std::ratio<1, 4>, gallons<>>)
	UNIT_ADD(volume, pints, pt, conversion_factor<std::ratio<1, 2>, quarts<>>)
	UNIT_ADD(volume, cups, c, conversion_factor<std::ratio<1, 2>, pints<>>)
	UNIT_ADD(volume, fluid_ounces, fl_oz, conversion_factor<std::ratio<1, 8>, cups<>>)
	UNIT_ADD(volume, barrels, bl, conversion_factor<std::ratio<42>, gallons<>>)
	UNIT_ADD(volume, bushels, bu, conversion_factor<std::ratio<215042, 100>, cubic_inches<>>)
	UNIT_ADD(volume, cords, cord, conversion_factor<std::ratio<128>, cubic_feet<>>)
	UNIT_ADD(volume, cubic_fathoms, fm3, cubed<fathoms<>>)
	UNIT_ADD(volume, tablespoons, tbsp, conversion_factor<std::ratio<1, 2>, fluid_ounces<>>)
	UNIT_ADD(volume, teaspoons, tsp, conversion_factor<std::ratio<1, 6>, fluid_ounces<>>)
	UNIT_ADD(volume, pinches, pinch, conversion_factor<std::ratio<1, 8>, teaspoons<>>)
	UNIT_ADD(volume, dashes, dash, conversion_factor<std::ratio<1, 2>, pinches<>>)
	UNIT_ADD(volume, drops, drop, conversion_factor<std::ratio<1, 360>, fluid_ounces<>>)
	UNIT_ADD(volume, fifths, fifth, conversion_factor<std::ratio<1, 5>, gallons<>>)
	UNIT_ADD(volume, drams, dr, conversion_factor<std::ratio<1, 8>, fluid_ounces<>>)
	UNIT_ADD(volume, gills, gi, conversion_factor<std::ratio<4>, fluid_ounces<>>)
	UNIT_ADD(volume, pecks, pk, conversion_factor<std::ratio<1, 4>, bushels<>>)
	UNIT_ADD(volume, sacks, sck, conversion_factor<std::ratio<3>, bushels<>>)
	UNIT_ADD(volume, shots, shts, conversion_factor<std::ratio<3, 2>, fluid_ounces<>>)
	UNIT_ADD(volume, strikes, strk, conversion_factor<std::ratio<2>, bushels<>>)

	UNIT_ADD_DIMENSION_TRAIT(volume)
} // namespace units

#endif // units_volume_h_