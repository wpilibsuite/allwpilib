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
/// @file	units/force.h
/// @brief	units representing force values
//
//--------------------------------------------------------------------------------------------------

#pragma once

#ifndef units_force_h_
#define units_force_h_

#include <units/acceleration.h>
#include <units/length.h>
#include <units/mass.h>
#include <units/time.h>

namespace units
{
	/**
	 * @namespace	units::force
	 * @brief		namespace for unit types and containers representing force values
	 * @details		The SI unit for force is `newtons`, and the corresponding `dimension` dimension is
	 *				`force_unit`.
	 * @anchor		forceContainers
	 * @sa			See unit for more information on unit type containers.
	 */
	UNIT_ADD_WITH_METRIC_PREFIXES(force, newtons, N, conversion_factor<std::ratio<1>, dimension::force>)
	UNIT_ADD(force, pounds, lbf, compound_conversion_factor<slugs_, feet_, inverse<squared<seconds_>>>)
	UNIT_ADD(force, dynes, dyn, conversion_factor<std::ratio<1, 100000>, newtons_>)
	UNIT_ADD(force, kiloponds, kp, compound_conversion_factor<standard_gravity_, kilograms_>)
	UNIT_ADD(force, poundals, pdl, compound_conversion_factor<mass::pounds_, feet_, inverse<squared<seconds_>>>)

	UNIT_ADD_DIMENSION_TRAIT(force)
} // namespace units

#endif // units_force_h_