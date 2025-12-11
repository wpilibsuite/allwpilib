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
/// @file	units/mass.h
/// @brief	units representing mass values
//
//--------------------------------------------------------------------------------------------------

#pragma once

#ifndef units_mass_h_
#define units_mass_h_

#include <wpi/units/core.h>

namespace wpi::units
{
	/**
	 * @namespace	wpi::units::mass
	 * @brief		namespace for unit types and containers representing mass values
	 * @details		The SI unit for mass is `kilograms`, and the corresponding `dimension` dimension is
	 *				`mass_unit`.
	 * @anchor		massContainers
	 * @sa			See unit for more information on unit type containers.
	 */
	UNIT_ADD_WITH_METRIC_PREFIXES(mass, grams, g, conversion_factor<std::ratio<1, 1000>, dimension::mass>)
	UNIT_ADD(mass, tonnes, t, conversion_factor<std::ratio<1000>, kilograms_>)
	UNIT_ADD(mass, pounds, lb, conversion_factor<std::ratio<45359237, 100000000>, kilograms_>)
	UNIT_ADD(mass, long_tons, ln_conversion_factor, conversion_factor<std::ratio<2240>, pounds_>)
	UNIT_ADD(mass, short_tons, sh_conversion_factor, conversion_factor<std::ratio<2000>, pounds_>)
	UNIT_ADD(mass, stone, st, conversion_factor<std::ratio<14>, pounds_>)
	UNIT_ADD(mass, ounces, oz, conversion_factor<std::ratio<1, 16>, pounds_>)
	UNIT_ADD(mass, carats, ct, conversion_factor<std::ratio<200>, milligrams_>)
	UNIT_ADD(mass, slugs, slug, conversion_factor<std::ratio<145939029, 10000000>, kilograms_>)

	UNIT_ADD_DIMENSION_TRAIT(mass)
} // namespace wpi::units

#endif // units_mass_h_