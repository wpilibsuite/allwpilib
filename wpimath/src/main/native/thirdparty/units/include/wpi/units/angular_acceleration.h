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
/// @file	units/angular_acceleration.h
/// @brief	units representing angular acceleration values
//
//--------------------------------------------------------------------------------------------------

#pragma once

#ifndef units_angular_acceleration_h_
#define units_angular_acceleration_h_

#include <wpi/units/angular_velocity.h>
#include <wpi/units/time.h>

namespace wpi::units
{
	/**
	 * @namespace	wpi::units::angular_acceleration
	 * @brief		namespace for unit types and containers representing angular acceleration values
	 * @details		The SI unit for angular acceleration is `radians_per_second_squared`, and the corresponding `dimension`
	 *dimension is `angular_acceleration_unit`.
	 * @anchor		angularAccelerationContainers
	 * @sa			See unit for more information on unit type containers.
	 */
	UNIT_ADD(angular_acceleration, radians_per_second_squared, rad_per_s_sq, conversion_factor<std::ratio<1>, dimension::angular_acceleration>)
	UNIT_ADD(angular_acceleration, degrees_per_second_squared, deg_per_s_sq, compound_conversion_factor<degrees_, inverse<squared<seconds_>>>)
	UNIT_ADD(angular_acceleration, turns_per_second_squared, tr_per_s_sq, compound_conversion_factor<turns_, inverse<squared<seconds_>>>)
	UNIT_ADD(angular_acceleration, revolutions_per_minute_squared, rev_per_m_sq, compound_conversion_factor<turns_, inverse<squared<minutes_>>>)
	UNIT_ADD(angular_acceleration, revolutions_per_minute_per_second, rev_per_m_per_s, compound_conversion_factor<turns_, inverse<minutes_>, inverse<seconds_>>)

	UNIT_ADD_DIMENSION_TRAIT(angular_acceleration)
} // namespace wpi::units

#endif // units_angular_acceleration_h_
