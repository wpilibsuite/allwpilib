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
/// @file	units/data.h
/// @brief	units representing data values
//
//--------------------------------------------------------------------------------------------------

#pragma once

#ifndef units_data_h_
#define units_data_h_

#include <wpi/units/core.hpp>

namespace wpi::units
{
	/**
	 * @namespace	wpi::units::data
	 * @brief		namespace for unit types and containers representing data values
	 * @details		The dimension for data is `bytes`, and the corresponding `dimension` dimension is
	 *				`data_unit`.
	 * @anchor		dataContainers
	 * @sa			See unit for more information on unit type containers.
	 */
	// Byte quantities are always at least one, so only the large decimal prefixes (kilo and up) and the
	// binary prefixes are meaningful; the sub-unit prefixes (deci/centi/milli/... and deca/hecto) are not
	// spelled out. This also frees the `_dB` literal for the dimensionless decibel, since `decibytes`
	// would otherwise claim it.
	UNIT_ADD(data, bytes, B, conversion_factor<std::ratio<1>, dimension::data>)
	UNIT_ADD(data, kilobytes, kB, kilo<bytes<>>)
	UNIT_ADD(data, megabytes, MB, mega<bytes<>>)
	UNIT_ADD(data, gigabytes, GB, giga<bytes<>>)
	UNIT_ADD(data, terabytes, TB, tera<bytes<>>)
	UNIT_ADD(data, petabytes, PB, peta<bytes<>>)
	UNIT_ADD(data, exabytes, EB, conversion_factor<std::ratio<1000>, petabytes_>)
	UNIT_ADD(data, kibibytes, KiB, kibi<bytes<>>)
	UNIT_ADD(data, mebibytes, MiB, mebi<bytes<>>)
	UNIT_ADD(data, gibibytes, GiB, gibi<bytes<>>)
	UNIT_ADD(data, tebibytes, TiB, tebi<bytes<>>)
	UNIT_ADD(data, pebibytes, PiB, pebi<bytes<>>)
	UNIT_ADD(data, exbibytes, EiB, exbi<bytes<>>)

	UNIT_ADD(data, bits, b, conversion_factor<std::ratio<1, 8>, bytes_>)
	UNIT_ADD(data, kilobits, kb, kilo<bits<>>)
	UNIT_ADD(data, megabits, Mb, mega<bits<>>)
	UNIT_ADD(data, gigabits, Gb, giga<bits<>>)
	UNIT_ADD(data, terabits, Tb, tera<bits<>>)
	UNIT_ADD(data, petabits, Pb, peta<bits<>>)
	UNIT_ADD(data, exabits, Eb, conversion_factor<std::ratio<1000>, petabits_>)
	UNIT_ADD(data, kibibits, Kib, kibi<bits<>>)
	UNIT_ADD(data, mebibits, Mib, mebi<bits<>>)
	UNIT_ADD(data, gibibits, Gib, gibi<bits<>>)
	UNIT_ADD(data, tebibits, Tib, tebi<bits<>>)
	UNIT_ADD(data, pebibits, Pib, pebi<bits<>>)
	UNIT_ADD(data, exbibits, Eib, exbi<bits<>>)

	UNIT_ADD(data, nibbles, nibble, conversion_factor<std::ratio<4>, bits_>)

	UNIT_ADD_DIMENSION_TRAIT(data, Data)
} // namespace wpi::units

#endif // units_data_h_