//--------------------------------------------------------------------------------------------------
//
//	UnitConversion: A compile-time c++23 unit conversion library with no dependencies
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
/// @file	units/viscosity.h
/// @brief	units representing dynamic and kinematic viscosity values
//
//--------------------------------------------------------------------------------------------------

#pragma once

#ifndef units_viscosity_h_
#define units_viscosity_h_

#include <wpi/units/area.hpp>
#include <wpi/units/pressure.hpp>
#include <wpi/units/time.hpp>

namespace wpi::units
{
	/**
	 * @namespace	wpi::units::dynamic_viscosity
	 * @brief		namespace for unit types and containers representing dynamic (absolute) viscosity values
	 * @details		The SI unit for dynamic viscosity is `pascal_seconds`, and the corresponding `dimension`
	 *				dimension is `dynamic_viscosity_unit`.
	 * @anchor		dynamicViscosityContainers
	 * @sa			See unit for more information on unit type containers.
	 */
	UNIT_ADD(dynamic_viscosity, pascal_seconds, Pa_s, compound_conversion_factor<pascals_, seconds_>)
	UNIT_ADD(dynamic_viscosity, poise, P, conversion_factor<std::ratio<1, 10>, pascal_seconds_>)
	UNIT_ADD(dynamic_viscosity, centipoise, cP, conversion_factor<std::ratio<1, 100>, poise_>)

	UNIT_ADD_DIMENSION_TRAIT(dynamic_viscosity, DynamicViscosity)

	/**
	 * @namespace	wpi::units::kinematic_viscosity
	 * @brief		namespace for unit types and containers representing kinematic viscosity values
	 * @details		The SI unit for kinematic viscosity is `square_meters_per_second`, and the corresponding
	 *				`dimension` dimension is `kinematic_viscosity_unit`.
	 * @anchor		kinematicViscosityContainers
	 * @sa			See unit for more information on unit type containers.
	 */
	UNIT_ADD(kinematic_viscosity, square_meters_per_second, m2_per_s, compound_conversion_factor<squared<meters<>>, inverse<seconds_>>)
	UNIT_ADD(kinematic_viscosity, stokes, St, conversion_factor<std::ratio<1, 10000>, square_meters_per_second_>)
	UNIT_ADD(kinematic_viscosity, centistokes, cSt, conversion_factor<std::ratio<1, 100>, stokes_>)

	UNIT_ADD_DIMENSION_TRAIT(kinematic_viscosity, KinematicViscosity)
} // namespace wpi::units

#endif // units_viscosity_h_
