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
/// @file	units/pressure.h
/// @brief	units representing pressure values
//
//--------------------------------------------------------------------------------------------------

#pragma once

#ifndef units_pressure_h_
#define units_pressure_h_

#ifdef _MSC_VER
#pragma push_macro("pascal")
#undef pascal
#endif // _MSC_VER

#if defined(__MINGW64__) || defined(__MINGW32__)
#	pragma push_macro("pascal")
#	undef pascal
#endif // __MINGW64__ or __MINGW32__

#include <wpi/units/force.hpp>
#include <wpi/units/length.hpp>

namespace wpi::units
{
	/**
	 * @namespace	wpi::units::pressure
	 * @brief		namespace for unit types and containers representing pressure values
	 * @details		The SI unit for pressure is `pascals`, and the corresponding `dimension` dimension is
	 *				`pressure_unit`.
	 * @anchor		pressureContainers
	 * @sa			See unit for more information on unit type containers.
	 */
	UNIT_ADD_WITH_METRIC_PREFIXES(pressure, pascals, Pa, conversion_factor<std::ratio<1>, dimension::pressure>)
	UNIT_ADD(pressure, bars, bar, conversion_factor<std::ratio<100>, kilo<pascals_>>)
	UNIT_ADD(pressure, millibars, mbar, conversion_factor<std::ratio<1>, milli<bars_>>)
	UNIT_ADD(pressure, atmospheres, atm, conversion_factor<std::ratio<101325>, pascals_>)
	UNIT_ADD(pressure, pounds_per_square_inch, psi, compound_conversion_factor<force::pounds_, inverse<squared<inches_>>>)
	UNIT_ADD(pressure, torrs, torr, conversion_factor<std::ratio<1, 760>, atmospheres_>)
	UNIT_ADD(pressure, millimeters_of_mercury, mmHg, conversion_factor<std::ratio<26664477483LL, 200000000LL>, pascals_>)
	UNIT_ADD(pressure, inches_of_mercury, inHg, conversion_factor<std::ratio<254, 10>, millimeters_of_mercury_>)

	UNIT_ADD(pressure, technical_atmospheres, at, conversion_factor<std::ratio<1961330, 20>, pascals_>)
	UNIT_ADD(pressure, pounds_per_square_foot, psf, compound_conversion_factor<force::pounds_, inverse<squared<feet_>>>)
	UNIT_ADD(pressure, kips_per_square_inch, ksi, conversion_factor<std::ratio<1000>, pounds_per_square_inch_>)
	UNIT_ADD(pressure, baryes, Ba, conversion_factor<std::ratio<1, 10>, pascals_>)
	UNIT_ADD(pressure, piezes, pz, conversion_factor<std::ratio<1000>, pascals_>)
	UNIT_ADD(pressure, centimeters_of_water, cmH2O, conversion_factor<std::ratio<1961330, 20000>, pascals_>)
	UNIT_ADD(pressure, millimeters_of_water, mmH2O, conversion_factor<std::ratio<1, 10>, centimeters_of_water_>)
	UNIT_ADD(pressure, inches_of_water, inH2O, conversion_factor<std::ratio<254, 100>, centimeters_of_water_>)

	UNIT_ADD_DIMENSION_TRAIT(pressure, Pressure)
} // namespace wpi::units

#ifdef _MSC_VER
#pragma pop_macro("pascal")
#endif // _MSC_VER

#if defined(__MINGW64__) || defined(__MINGW32__)
#	pragma pop_macro("pascal")
#endif // __MINGW64__ or __MINGW32_

#endif // units_pressure_h_