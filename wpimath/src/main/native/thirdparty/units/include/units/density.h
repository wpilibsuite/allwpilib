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
/// @file	units/density.h
/// @brief	units representing density values
//
//--------------------------------------------------------------------------------------------------

#pragma once

#ifndef units_density_h_
#define units_density_h_

#include <units/mass.h>
#include <units/volume.h>

namespace units
{
	/**
	 * @namespace	units::density
	 * @brief		namespace for unit types and containers representing density values
	 * @details		The SI unit for density is `kilograms_per_cubic_meter`, and the corresponding `dimension` dimension
	 *is `density_unit`.
	 * @anchor		densityContainers
	 * @sa			See unit for more information on unit type containers.
	 */
	UNIT_ADD(density, kilograms_per_cubic_meter, kg_per_m3, conversion_factor<std::ratio<1>, dimension::density>)
	UNIT_ADD(density, grams_per_milliliter, g_per_mL, compound_conversion_factor<grams_, inverse<milliliters_>>)
	UNIT_ADD(density, kilograms_per_liter, kg_per_L, conversion_factor<std::ratio<1>, compound_conversion_factor<grams_, inverse<milliliters_>>>)
	UNIT_ADD(density, ounces_per_cubic_foot, oz_per_ft3, compound_conversion_factor<ounces_, inverse<cubic_feet_>>)
	UNIT_ADD(density, ounces_per_cubic_inch, oz_per_in3, compound_conversion_factor<ounces_, inverse<cubic_inches<>>>)
	UNIT_ADD(density, ounces_per_gallon, oz_per_gal, compound_conversion_factor<ounces<>, inverse<gallons<>>>)
	UNIT_ADD(density, pounds_per_cubic_foot, lb_per_ft3, compound_conversion_factor<mass::pounds<>, inverse<cubic_feet<>>>)
	UNIT_ADD(density, pounds_per_cubic_inch, lb_per_in3, compound_conversion_factor<mass::pounds<>, inverse<cubic_inches<>>>)
	UNIT_ADD(density, pounds_per_gallon, lb_per_gal, compound_conversion_factor<mass::pounds<>, inverse<gallons<>>>)
	UNIT_ADD(density, slugs_per_cubic_foot, slug_per_ft3, compound_conversion_factor<slugs<>, inverse<cubic_feet<>>>)

	UNIT_ADD_DIMENSION_TRAIT(density)
} // namespace units

#endif // units_density_h_