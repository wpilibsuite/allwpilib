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
/// @file	units/angular_velocity.h
/// @brief	units representing angular velocity values
//
//--------------------------------------------------------------------------------------------------

#pragma once

#ifndef units_angular_velocity_h_
#define units_angular_velocity_h_

#include <wpi/units/angle.h>
#include <wpi/units/time.h>

namespace wpi::units
{
	/**
	 * @namespace	wpi::units::angular_velocity
	 * @brief		namespace for unit types and containers representing angular velocity values
	 * @details		The SI unit for angular velocity is `radians_per_second`, and the corresponding `dimension`
	 *				dimension is `angular_velocity_unit`.
	 * @anchor		angularVelocityContainers
	 * @sa			See unit for more information on unit type containers.
	 */
	UNIT_ADD(angular_velocity, radians_per_second, rad_per_s, conversion_factor<std::ratio<1>, dimension::angular_velocity>)
	UNIT_ADD(angular_velocity, degrees_per_second, deg_per_s, compound_conversion_factor<degrees_, inverse<seconds_>>)
	UNIT_ADD(angular_velocity, revolutions_per_minute, rpm, conversion_factor<std::ratio<2, 60>, radians_per_second_, std::ratio<1>>)
	UNIT_ADD(angular_velocity, revolutions_per_second, rps, conversion_factor<std::ratio<2, 1>, radians_per_second<>, std::ratio<1>>)
	UNIT_ADD(angular_velocity, milliarcseconds_per_year, mas_per_yr, compound_conversion_factor<milliarcseconds<>, inverse<years<>>>)

	UNIT_ADD_DIMENSION_TRAIT(angular_velocity)
} // namespace wpi::units

#endif // units_angular_velocity_h_