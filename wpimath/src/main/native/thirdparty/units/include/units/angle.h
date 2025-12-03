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
/// @file	units/pressure.h
/// @brief	units representing angle values
//
//--------------------------------------------------------------------------------------------------

#pragma once

#ifndef units_angle_h_
#define units_angle_h_

#include <units/core.h>

namespace units
{
	/**
	 * @namespace	units::angle
	 * @brief		namespace for unit types and containers representing angle values
	 * @details		The SI unit for angle is `radians`, and the corresponding `dimension` dimension is
	 *				`angle_unit`.
	 * @anchor		angleContainers
	 * @sa			See unit for more information on unit type containers.
	 */
	UNIT_ADD_WITH_METRIC_PREFIXES(angle, radians, rad, conversion_factor<std::ratio<1>, dimension::angle>)
	UNIT_ADD(angle, degrees, deg, conversion_factor<std::ratio<1, 180>, radians<>, std::ratio<1>>)
	UNIT_ADD(angle, arcminutes, arcmin, conversion_factor<std::ratio<1, 60>, degrees<>>)
	UNIT_ADD(angle, arcseconds, arcsec, conversion_factor<std::ratio<1, 60>, arcminutes<>>)
	UNIT_ADD(angle, milliarcseconds, mas, milli<arcseconds<>>)
	UNIT_ADD(angle, turns, tr, conversion_factor<std::ratio<2>, radians<>, std::ratio<1>>)
	UNIT_ADD(angle, gradians, gon, conversion_factor<std::ratio<1, 400>, turns<>>)

	UNIT_ADD_DIMENSION_TRAIT(angle)

	//----------------------------------
	//	UNIT-ENABLED CMATH FUNCTIONS
	//----------------------------------

	//----------------------------------
	//	TRIGONOMETRIC FUNCTIONS
	//----------------------------------

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute cosine
	 * @details		The input value can be in any unit of angle, including radians or degrees.
	 * @tparam		AngleUnit	any `unit` type of `dimension::angle`.
	 * @param[in]	angle		angle to compute the cosine of
	 * @returns		Returns the cosine of <i>angle</i>
	 */
	template<class AngleUnit, std::enable_if_t<traits::is_angle_unit_v<AngleUnit>, int> = 0>
	dimensionless<detail::floating_point_promotion_t<typename AngleUnit::underlying_type>> cos(const AngleUnit angle) noexcept
	{
		return std::cos(convert<radians<detail::floating_point_promotion_t<typename AngleUnit::underlying_type>>>(angle).value());
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute sine
	 * @details		The input value can be in any unit of angle, including radians or degrees.
	 * @tparam		AngleUnit	any `unit` type of `dimension::angle`.
	 * @param[in]	angle		angle to compute the since of
	 * @returns		Returns the sine of <i>angle</i>
	 */
	template<class AngleUnit, std::enable_if_t<traits::is_angle_unit_v<AngleUnit>, int> = 0>
	dimensionless<detail::floating_point_promotion_t<typename AngleUnit::underlying_type>> sin(const AngleUnit angle) noexcept
	{
		return std::sin(convert<radians<detail::floating_point_promotion_t<typename AngleUnit::underlying_type>>>(angle).value());
	}
	/**
	 * @ingroup		UnitMath
	 * @brief		Compute tangent
	 * @details		The input value can be in any unit of angle, including radians or degrees.
	 * @tparam		AngleUnit	any `unit` type of `dimension::angle`.
	 * @param[in]	angle		angle to compute the tangent of
	 * @returns		Returns the tangent of <i>angle</i>
	 */
	template<class AngleUnit, std::enable_if_t<traits::is_angle_unit_v<AngleUnit>, int> = 0>
	dimensionless<detail::floating_point_promotion_t<typename AngleUnit::underlying_type>> tan(const AngleUnit angle) noexcept
	{
		return std::tan(convert<radians<detail::floating_point_promotion_t<typename AngleUnit::underlying_type>>>(angle).value());
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute arc cosine
	 * @details		Returns the principal value of the arc cosine of x, expressed in radians.
	 * @param[in]	x		Value whose arc cosine is computed, in the interval [-1,+1].
	 * @returns		Principal arc cosine of x, in the interval [0,pi] radians.
	 */
	template<class dimensionlessUnit, std::enable_if_t<traits::is_dimensionless_unit_v<dimensionlessUnit>, int> = 0>
	radians<detail::floating_point_promotion_t<typename dimensionlessUnit::underlying_type>> acos(const dimensionlessUnit x) noexcept
	{
		return radians<detail::floating_point_promotion_t<typename dimensionlessUnit::underlying_type>>(
			std::acos(x.template to<typename dimensionlessUnit::underlying_type>()));
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute arc sine
	 * @details		Returns the principal value of the arc sine of x, expressed in radians.
	 * @param[in]	x		Value whose arc sine is computed, in the interval [-1,+1].
	 * @returns		Principal arc sine of x, in the interval [-pi/2,+pi/2] radians.
	 */
	template<class dimensionlessUnit, std::enable_if_t<traits::is_dimensionless_unit_v<dimensionlessUnit>, int> = 0>
	radians<detail::floating_point_promotion_t<typename dimensionlessUnit::underlying_type>> asin(const dimensionlessUnit x) noexcept
	{
		return radians<detail::floating_point_promotion_t<typename dimensionlessUnit::underlying_type>>(
			std::asin(x.template to<typename dimensionlessUnit::underlying_type>()));
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute arc tangent
	 * @details		Returns the principal value of the arc tangent of x, expressed in radians.
	 *				Notice that because of the sign ambiguity, the function cannot determine with
	 *				certainty in which quadrant the angle falls only by its tangent value. See
	 *				atan2 for an alternative that takes a fractional argument instead.
	 * @tparam		AngleUnit	any `unit` type of `dimension::angle`.
	 * @param[in]	x		Value whose arc tangent is computed, in the interval [-1,+1].
	 * @returns		Principal arc tangent of x, in the interval [-pi/2,+pi/2] radians.
	 */
	template<class dimensionlessUnit, std::enable_if_t<traits::is_dimensionless_unit_v<dimensionlessUnit>, int> = 0>
	radians<detail::floating_point_promotion_t<typename dimensionlessUnit::underlying_type>> atan(const dimensionlessUnit x) noexcept
	{
		return radians<detail::floating_point_promotion_t<typename dimensionlessUnit::underlying_type>>(
			std::atan(x.template to<typename dimensionlessUnit::underlying_type>()));
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute arc tangent with two parameters
	 * @details		To compute the value, the function takes into account the sign of both arguments in order to
	 *				determine the quadrant.
	 * @param[in]	y		y-component of the triangle expressed.
	 * @param[in]	x		x-component of the triangle expressed.
	 * @returns		Returns the principal value of the arc tangent of <i>y/x</i>, expressed in radians.
	 */
	template<class Y, class X, std::enable_if_t<traits::is_dimensionless_unit_v<decltype(std::declval<Y>() / std::declval<X>())>, int> = 0>
	radians<detail::floating_point_promotion_t<std::common_type_t<typename X::underlying_type, typename Y::underlying_type>>> atan2(
		const Y y, const X x) noexcept
	{
		using CommonUnit = std::common_type_t<X, Y>;
		// X and Y could be different length units, so normalize them
		return radians<detail::floating_point_promotion_t<typename CommonUnit::underlying_type>>(std::atan2(CommonUnit(y).value(), CommonUnit(x).value()));
	}

	//----------------------------------
	//	HYPERBOLIC TRIG FUNCTIONS
	//----------------------------------

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute hyperbolic cosine
	 * @details		The input value can be in any unit of angle, including radians or degrees.
	 * @tparam		AngleUnit	any `unit` type of `dimension::angle`.
	 * @param[in]	angle		angle to compute the hyperbolic cosine of
	 * @returns		Returns the hyperbolic cosine of <i>angle</i>
	 */
	template<class AngleUnit, std::enable_if_t<traits::is_angle_unit_v<AngleUnit>, int> = 0>
	dimensionless<detail::floating_point_promotion_t<typename AngleUnit::underlying_type>> cosh(const AngleUnit angle) noexcept
	{
		return std::cosh(convert<radians<detail::floating_point_promotion_t<typename AngleUnit::underlying_type>>>(angle).value());
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute hyperbolic sine
	 * @details		The input value can be in any unit of angle, including radians or degrees.
	 * @tparam		AngleUnit	any `unit` type of `dimension::angle`.
	 * @param[in]	angle		angle to compute the hyperbolic sine of
	 * @returns		Returns the hyperbolic sine of <i>angle</i>
	 */
	template<class AngleUnit, std::enable_if_t<traits::is_angle_unit_v<AngleUnit>, int> = 0>
	dimensionless<detail::floating_point_promotion_t<typename AngleUnit::underlying_type>> sinh(const AngleUnit angle) noexcept
	{
		return std::sinh(convert<radians<detail::floating_point_promotion_t<typename AngleUnit::underlying_type>>>(angle).value());
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute hyperbolic tangent
	 * @details		The input value can be in any unit of angle, including radians or degrees.
	 * @tparam		AngleUnit	any `unit` type of `dimension::angle`.
	 * @param[in]	angle		angle to compute the hyperbolic tangent of
	 * @returns		Returns the hyperbolic tangent of <i>angle</i>
	 */
	template<class AngleUnit, std::enable_if_t<traits::is_angle_unit_v<AngleUnit>, int> = 0>
	dimensionless<detail::floating_point_promotion_t<typename AngleUnit::underlying_type>> tanh(const AngleUnit angle) noexcept
	{
		return std::tanh(convert<radians<detail::floating_point_promotion_t<typename AngleUnit::underlying_type>>>(angle).value());
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute arc hyperbolic cosine
	 * @details		Returns the nonnegative arc hyperbolic cosine of x, expressed in radians.
	 * @param[in]	x	Value whose arc hyperbolic cosine is computed. If the argument is less
	 *					than 1, a domain error occurs.
	 * @returns		Nonnegative arc hyperbolic cosine of x, in the interval [0,+INFINITY] radians.
	 */
	template<class dimensionlessUnit, std::enable_if_t<traits::is_dimensionless_unit_v<dimensionlessUnit>, int> = 0>
	radians<detail::floating_point_promotion_t<typename dimensionlessUnit::underlying_type>> acosh(const dimensionlessUnit x) noexcept
	{
		return radians<detail::floating_point_promotion_t<typename dimensionlessUnit::underlying_type>>(
			std::acosh(x.template to<typename dimensionlessUnit::underlying_type>()));
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute arc hyperbolic sine
	 * @details		Returns the arc hyperbolic sine of x, expressed in radians.
	 * @param[in]	x	Value whose arc hyperbolic sine is computed.
	 * @returns		Arc hyperbolic sine of x, in radians.
	 */
	template<class dimensionlessUnit, std::enable_if_t<traits::is_dimensionless_unit_v<dimensionlessUnit>, int> = 0>
	radians<detail::floating_point_promotion_t<typename dimensionlessUnit::underlying_type>> asinh(const dimensionlessUnit x) noexcept
	{
		return radians<detail::floating_point_promotion_t<typename dimensionlessUnit::underlying_type>>(
			std::asinh(x.template to<typename dimensionlessUnit::underlying_type>()));
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute arc hyperbolic tangent
	 * @details		Returns the arc hyperbolic tangent of x, expressed in radians.
	 * @param[in]	x	Value whose arc hyperbolic tangent is computed, in the interval [-1,+1].
	 *					If the argument is out of this interval, a domain error occurs. For
	 *					values of -1 and +1, a pole error may occur.
	 * @returns		units::angle::radian
	 */
	template<class dimensionlessUnit, std::enable_if_t<traits::is_dimensionless_unit_v<dimensionlessUnit>, int> = 0>
	radians<detail::floating_point_promotion_t<typename dimensionlessUnit::underlying_type>> atanh(const dimensionlessUnit x) noexcept
	{
		return radians<detail::floating_point_promotion_t<typename dimensionlessUnit::underlying_type>>(
			std::atanh(x.template to<typename dimensionlessUnit::underlying_type>()));
	}
} // namespace units

#endif // units_angle_h_