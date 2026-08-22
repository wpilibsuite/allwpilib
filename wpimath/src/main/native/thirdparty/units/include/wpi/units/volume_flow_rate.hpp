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
/// @file	units/volume_flow_rate.h
/// @brief	units representing volumetric flow rate values
//
//--------------------------------------------------------------------------------------------------

#pragma once

#ifndef units_volume_flow_rate_h_
#define units_volume_flow_rate_h_

#include <wpi/units/time.hpp>
#include <wpi/units/volume.hpp>

namespace wpi::units
{
	/**
	 * @namespace	wpi::units::volume_flow_rate
	 * @brief		namespace for unit types and containers representing volumetric-flow-rate values
	 * @details		The SI unit for volumetric flow rate is `cubic_meters_per_second`, and the corresponding
	 *				`dimension` dimension is `volume_flow_rate_unit`.
	 * @anchor		volumeFlowRateContainers
	 * @sa			See unit for more information on unit type containers.
	 */
	UNIT_ADD(volume_flow_rate, cubic_meters_per_second, m3_per_s, conversion_factor<std::ratio<1>, dimension::volume_flow_rate>)
	UNIT_ADD(volume_flow_rate, cubic_meters_per_hour, m3_per_hr, compound_conversion_factor<cubic_meters<>, inverse<hours<>>>)
	UNIT_ADD(volume_flow_rate, liters_per_second, L_per_s, compound_conversion_factor<liters<>, inverse<seconds_>>)
	UNIT_ADD(volume_flow_rate, liters_per_minute, L_per_min, compound_conversion_factor<liters<>, inverse<minutes_>>)
	UNIT_ADD(volume_flow_rate, gallons_per_minute, gpm, compound_conversion_factor<gallons<>, inverse<minutes_>>)
	UNIT_ADD(volume_flow_rate, gallons_per_hour, gph, compound_conversion_factor<gallons<>, inverse<hours<>>>)
	UNIT_ADD(volume_flow_rate, cubic_feet_per_second, cfs, compound_conversion_factor<cubic_feet<>, inverse<seconds_>>)
	UNIT_ADD(volume_flow_rate, cubic_feet_per_minute, cfm, compound_conversion_factor<cubic_feet<>, inverse<minutes_>>)

	UNIT_ADD_DIMENSION_TRAIT(volume_flow_rate, VolumeFlowRate)
} // namespace wpi::units

#endif // units_volume_flow_rate_h_
