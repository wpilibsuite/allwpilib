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
// https://en.wikipedia.org/wiki/Luminance
// https://github.com/swatanabe/cppnow17-units
//
//--------------------------------------------------------------------------------------------------
//
/// @file	units/luminance.h
/// @brief	units representing luminance values
//
//--------------------------------------------------------------------------------------------------

#pragma once

#ifndef units_luminance_h_
#define units_luminance_h__

#include <units/length.h>
#include <units/luminous_intensity.h>

namespace units
{
	/**
	 * @namespace	units::luminance
	 * @brief		namespace for unit types and containers representing luminance values
	 * @details		The SI unit for illuminance is `candelas per square meter`, and the corresponding `dimension` dimension is
	 *				`luminance_unit`.
	 * @anchor		luminanceContainers
	 * @sa			See unit for more information on unit type containers.
	 */
	UNIT_ADD_WITH_METRIC_PREFIXES(luminance, candelas_per_square_meter, cd_per_m2, conversion_factor<std::ratio<1>, dimension::luminance>)
	UNIT_ADD(luminance, stilbs, sb, conversion_factor<std::ratio<10'000>, candelas_per_square_meter_>)
	UNIT_ADD(luminance, apostilbs, asb, conversion_factor<std::ratio<1>, candelas_per_square_meter_, std::ratio<-1>>)
	UNIT_ADD(luminance, brils, bril, conversion_factor<std::ratio<1, 10'000'000>, candelas_per_square_meter<>, std::ratio<-1>>)
	UNIT_ADD(luminance, skots, sk, conversion_factor<std::ratio<1, 1'000>, candelas_per_square_meter<>, std::ratio<-1>>)
	UNIT_ADD(luminance, lamberts, la, conversion_factor<std::ratio<1>, stilbs<>, std::ratio<-1>>)
	UNIT_ADD(luminance, millilamberts, mla, conversion_factor<std::ratio<1, 1'000>, lamberts<>>)
	UNIT_ADD(luminance, foot_lamberts, ftL, compound_conversion_factor<conversion_factor<std::ratio<1>, dimension::dimensionless, std::ratio<-1>>, candelas<>, inverse<squared<feet<>>>>)

	// Aliases
	template<class T>
	using nits = candelas_per_square_meter<T>;

	template<class T>
	using blondels = apostilbs<T>;

	UNIT_ADD_DIMENSION_TRAIT(luminance)
} // namespace units

#endif // units_luminance_h__