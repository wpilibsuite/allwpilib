// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

// Copyright (c) 2016 Nic Holthaus
//
// The MIT License (MIT)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <cmath>

#include <wpi/math>

#include "units/angle.h"
#include "units/base.h"
#include "units/dimensionless.h"

//----------------------------------
// UNIT-ENABLED CMATH FUNCTIONS
//----------------------------------

/**
 * @brief namespace for unit-enabled versions of the `<cmath>` library
 * @details Includes trigonometric functions, exponential/log functions,
 *          rounding functions, etc.
 * @sa See `unit_t` for more information on unit type containers.
 */
namespace units::math {
//----------------------------------
// TRIGONOMETRIC FUNCTIONS
//----------------------------------

/**
 * @ingroup UnitMath
 * @brief Compute cosine
 * @details The input value can be in any unit of angle, including radians or
 *          degrees.
 * @tparam AngleUnit any `unit_t` type of `category::angle_unit`.
 * @param[in] angle angle to compute the cosine of
 * @returns Returns the cosine of <i>angle</i>
 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_ANGLE_UNITS)
template <class AngleUnit>
dimensionless::scalar_t cos(const AngleUnit angle) noexcept {
  static_assert(
      traits::is_angle_unit<AngleUnit>::value,
      "Type `AngleUnit` must be a unit of angle derived from `unit_t`.");
  return dimensionless::scalar_t(
      std::cos(angle.template convert<angle::radian>()()));
}
#endif

/**
 * @ingroup UnitMath
 * @brief Compute sine
 * @details The input value can be in any unit of angle, including radians or
 *          degrees.
 * @tparam AngleUnit  any `unit_t` type of `category::angle_unit`.
 * @param[in] angle angle to compute the since of
 * @returns Returns the sine of <i>angle</i>
 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_ANGLE_UNITS)
template <class AngleUnit>
dimensionless::scalar_t sin(const AngleUnit angle) noexcept {
  static_assert(
      traits::is_angle_unit<AngleUnit>::value,
      "Type `AngleUnit` must be a unit of angle derived from `unit_t`.");
  return dimensionless::scalar_t(
      std::sin(angle.template convert<angle::radian>()()));
}
#endif
/**
 * @ingroup UnitMath
 * @brief Compute tangent
 * @details The input value can be in any unit of angle, including radians or
 *          degrees.
 * @tparam AngleUnit  any `unit_t` type of `category::angle_unit`.
 * @param[in] angle angle to compute the tangent of
 * @returns Returns the tangent of <i>angle</i>
 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_ANGLE_UNITS)
template <class AngleUnit>
dimensionless::scalar_t tan(const AngleUnit angle) noexcept {
  static_assert(
      traits::is_angle_unit<AngleUnit>::value,
      "Type `AngleUnit` must be a unit of angle derived from `unit_t`.");
  return dimensionless::scalar_t(
      std::tan(angle.template convert<angle::radian>()()));
}
#endif

/**
 * @ingroup UnitMath
 * @brief Compute arc cosine
 * @details Returns the principal value of the arc cosine of x, expressed in
 *          radians.
 * @param[in] x Value whose arc cosine is computed, in the interval [-1,+1].
 * @returns Principal arc cosine of x, in the interval [0,pi] radians.
 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_ANGLE_UNITS)
template <class ScalarUnit>
angle::radian_t acos(const ScalarUnit x) noexcept {
  static_assert(
      traits::is_dimensionless_unit<ScalarUnit>::value,
      "Type `ScalarUnit` must be a dimensionless unit derived from `unit_t`.");
  return angle::radian_t(std::acos(x()));
}
#endif

/**
 * @ingroup UnitMath
 * @brief Compute arc sine
 * @details Returns the principal value of the arc sine of x, expressed in
 *          radians.
 * @param[in] x Value whose arc sine is computed, in the interval [-1,+1].
 * @returns Principal arc sine of x, in the interval [-pi/2,+pi/2] radians.
 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_ANGLE_UNITS)
template <class ScalarUnit>
angle::radian_t asin(const ScalarUnit x) noexcept {
  static_assert(
      traits::is_dimensionless_unit<ScalarUnit>::value,
      "Type `ScalarUnit` must be a dimensionless unit derived from `unit_t`.");
  return angle::radian_t(std::asin(x()));
}
#endif

/**
 * @ingroup UnitMath
 * @brief Compute arc tangent
 * @details Returns the principal value of the arc tangent of x, expressed in
 *          radians. Notice that because of the sign ambiguity, the function
 *          cannot determine with certainty in which quadrant the angle falls
 *          only by its tangent value. See atan2 for an alternative that takes a
 *          fractional argument instead.
 * @tparam AngleUnit  any `unit_t` type of `category::angle_unit`.
 * @param[in] x Value whose arc tangent is computed, in the interval [-1,+1].
 * @returns Principal arc tangent of x, in the interval [-pi/2,+pi/2] radians.
 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_ANGLE_UNITS)
template <class ScalarUnit>
angle::radian_t atan(const ScalarUnit x) noexcept {
  static_assert(
      traits::is_dimensionless_unit<ScalarUnit>::value,
      "Type `ScalarUnit` must be a dimensionless unit derived from `unit_t`.");
  return angle::radian_t(std::atan(x()));
}
#endif

/**
 * @ingroup UnitMath
 * @brief Compute arc tangent with two parameters
 * @details To compute the value, the function takes into account the sign of
 *          both arguments in order to determine the quadrant.
 * @param[in] y y-component of the triangle expressed.
 * @param[in] x x-component of the triangle expressed.
 * @returns Returns the principal value of the arc tangent of <i>y/x</i>,
 *          expressed in radians.
 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_ANGLE_UNITS)
template <class Y, class X>
angle::radian_t atan2(const Y y, const X x) noexcept {
  static_assert(traits::is_dimensionless_unit<decltype(y / x)>::value,
                "The quantity y/x must yield a dimensionless ratio.");

  // X and Y could be different length units, so normalize them
  return angle::radian_t(
      std::atan2(y.template convert<
                     typename units::traits::unit_t_traits<X>::unit_type>()(),
                 x()));
}
#endif

//----------------------------------
// HYPERBOLIC TRIG FUNCTIONS
//----------------------------------

/**
 * @ingroup UnitMath
 * @brief Compute hyperbolic cosine
 * @details The input value can be in any unit of angle, including radians or
 *          degrees.
 * @tparam AngleUnit any `unit_t` type of `category::angle_unit`.
 * @param[in] angle angle to compute the hyperbolic cosine of
 * @returns Returns the hyperbolic cosine of <i>angle</i>
 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_ANGLE_UNITS)
template <class AngleUnit>
dimensionless::scalar_t cosh(const AngleUnit angle) noexcept {
  static_assert(
      traits::is_angle_unit<AngleUnit>::value,
      "Type `AngleUnit` must be a unit of angle derived from `unit_t`.");
  return dimensionless::scalar_t(
      std::cosh(angle.template convert<angle::radian>()()));
}
#endif

/**
 * @ingroup UnitMath
 * @brief Compute hyperbolic sine
 * @details The input value can be in any unit of angle, including radians or
 *          degrees.
 * @tparam AngleUnit any `unit_t` type of `category::angle_unit`.
 * @param[in] angle angle to compute the hyperbolic sine of
 * @returns Returns the hyperbolic sine of <i>angle</i>
 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_ANGLE_UNITS)
template <class AngleUnit>
dimensionless::scalar_t sinh(const AngleUnit angle) noexcept {
  static_assert(
      traits::is_angle_unit<AngleUnit>::value,
      "Type `AngleUnit` must be a unit of angle derived from `unit_t`.");
  return dimensionless::scalar_t(
      std::sinh(angle.template convert<angle::radian>()()));
}
#endif

/**
 * @ingroup UnitMath
 * @brief Compute hyperbolic tangent
 * @details The input value can be in any unit of angle, including radians or
 *          degrees.
 * @tparam AngleUnit any `unit_t` type of `category::angle_unit`.
 * @param[in] angle angle to compute the hyperbolic tangent of
 * @returns Returns the hyperbolic tangent of <i>angle</i>
 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_ANGLE_UNITS)
template <class AngleUnit>
dimensionless::scalar_t tanh(const AngleUnit angle) noexcept {
  static_assert(
      traits::is_angle_unit<AngleUnit>::value,
      "Type `AngleUnit` must be a unit of angle derived from `unit_t`.");
  return dimensionless::scalar_t(
      std::tanh(angle.template convert<angle::radian>()()));
}
#endif

/**
 * @ingroup UnitMath
 * @brief Compute arc hyperbolic cosine
 * @details Returns the nonnegative arc hyperbolic cosine of x, expressed in
 *          radians.
 * @param[in] x Value whose arc hyperbolic cosine is computed. If the argument
 *              is less than 1, a domain error occurs.
 * @returns Nonnegative arc hyperbolic cosine of x, in the interval
 *          [0,+INFINITY] radians.
 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_ANGLE_UNITS)
template <class ScalarUnit>
angle::radian_t acosh(const ScalarUnit x) noexcept {
  static_assert(
      traits::is_dimensionless_unit<ScalarUnit>::value,
      "Type `ScalarUnit` must be a dimensionless unit derived from `unit_t`.");
  return angle::radian_t(std::acosh(x()));
}
#endif

/**
 * @ingroup UnitMath
 * @brief Compute arc hyperbolic sine
 * @details Returns the arc hyperbolic sine of x, expressed in radians.
 * @param[in] x Value whose arc hyperbolic sine is computed.
 * @returns Arc hyperbolic sine of x, in radians.
 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_ANGLE_UNITS)
template <class ScalarUnit>
angle::radian_t asinh(const ScalarUnit x) noexcept {
  static_assert(
      traits::is_dimensionless_unit<ScalarUnit>::value,
      "Type `ScalarUnit` must be a dimensionless unit derived from `unit_t`.");
  return angle::radian_t(std::asinh(x()));
}
#endif

/**
 * @ingroup UnitMath
 * @brief Compute arc hyperbolic tangent
 * @details Returns the arc hyperbolic tangent of x, expressed in radians.
 * @param[in] x Value whose arc hyperbolic tangent is computed, in the interval
 *              [-1,+1]. If the argument is out of this interval, a domain error
 *              occurs. For values of -1 and +1, a pole error may occur.
 * @returns units::angle::radian_t
 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_ANGLE_UNITS)
template <class ScalarUnit>
angle::radian_t atanh(const ScalarUnit x) noexcept {
  static_assert(
      traits::is_dimensionless_unit<ScalarUnit>::value,
      "Type `ScalarUnit` must be a dimensionless unit derived from `unit_t`.");
  return angle::radian_t(std::atanh(x()));
}
#endif

//----------------------------------
// TRANSCENDENTAL FUNCTIONS
//----------------------------------

// it makes NO SENSE to put dimensioned units into a transcendental function,
// and if you think it does you are demonstrably wrong.
// https://en.wikipedia.org/wiki/Transcendental_function#Dimensional_analysis

/**
 * @ingroup UnitMath
 * @brief Compute exponential function
 * @details Returns the base-e exponential function of x, which is e raised to
 *          the power x: ex.
 * @param[in] x scalar value of the exponent.
 * @returns Exponential value of x.
 *          If the magnitude of the result is too large to be represented by a
 *          value of the return type, the function returns HUGE_VAL (or
 *          HUGE_VALF or HUGE_VALL) with the proper sign, and an overflow range
 *          error occurs.
 */
template <class ScalarUnit>
dimensionless::scalar_t exp(const ScalarUnit x) noexcept {
  static_assert(
      traits::is_dimensionless_unit<ScalarUnit>::value,
      "Type `ScalarUnit` must be a dimensionless unit derived from `unit_t`.");
  return dimensionless::scalar_t(std::exp(x()));
}

/**
 * @ingroup UnitMath
 * @brief Compute natural logarithm
 * @details Returns the natural logarithm of x.
 * @param[in] x scalar value whose logarithm is calculated. If the argument is
 *              negative, a domain error occurs.
 * @sa log10 for more common base-10 logarithms
 * @returns Natural logarithm of x.
 */
template <class ScalarUnit>
dimensionless::scalar_t log(const ScalarUnit x) noexcept {
  static_assert(
      traits::is_dimensionless_unit<ScalarUnit>::value,
      "Type `ScalarUnit` must be a dimensionless unit derived from `unit_t`.");
  return dimensionless::scalar_t(std::log(x()));
}

/**
 * @ingroup UnitMath
 * @brief Compute common logarithm
 * @details Returns the common (base-10) logarithm of x.
 * @param[in] x Value whose logarithm is calculated. If the argument is
 *              negative, a domain error occurs.
 * @returns Common logarithm of x.
 */
template <class ScalarUnit>
dimensionless::scalar_t log10(const ScalarUnit x) noexcept {
  static_assert(
      traits::is_dimensionless_unit<ScalarUnit>::value,
      "Type `ScalarUnit` must be a dimensionless unit derived from `unit_t`.");
  return dimensionless::scalar_t(std::log10(x()));
}

/**
 * @ingroup UnitMath
 * @brief Break into fractional and integral parts.
 * @details The integer part is stored in the object pointed by intpart, and the
 *          fractional part is returned by the function. Both parts have the
 *          same sign as x.
 * @param[in] x scalar value to break into parts.
 * @param[in] intpart Pointer to an object (of the same type as x) where the
 *                    integral part is stored with the same sign as x.
 * @returns The fractional part of x, with the same sign.
 */
template <class ScalarUnit>
dimensionless::scalar_t modf(const ScalarUnit x, ScalarUnit* intpart) noexcept {
  static_assert(
      traits::is_dimensionless_unit<ScalarUnit>::value,
      "Type `ScalarUnit` must be a dimensionless unit derived from `unit_t`.");

  UNIT_LIB_DEFAULT_TYPE intp;
  dimensionless::scalar_t fracpart =
      dimensionless::scalar_t(std::modf(x(), &intp));
  *intpart = intp;
  return fracpart;
}

/**
 * @ingroup UnitMath
 * @brief Compute binary exponential function
 * @details Returns the base-2 exponential function of x, which is 2 raised to
 *          the power x: 2^x. 2param[in]  x  Value of the exponent.
 * @returns 2 raised to the power of x.
 */
template <class ScalarUnit>
dimensionless::scalar_t exp2(const ScalarUnit x) noexcept {
  static_assert(
      traits::is_dimensionless_unit<ScalarUnit>::value,
      "Type `ScalarUnit` must be a dimensionless unit derived from `unit_t`.");
  return dimensionless::scalar_t(std::exp2(x()));
}

/**
 * @ingroup UnitMath
 * @brief Compute exponential minus one
 * @details Returns e raised to the power x minus one: e^x-1. For small
 *          magnitude values of x, expm1 may be more accurate than exp(x)-1.
 * @param[in] x Value of the exponent.
 * @returns e raised to the power of x, minus one.
 */
template <class ScalarUnit>
dimensionless::scalar_t expm1(const ScalarUnit x) noexcept {
  static_assert(
      traits::is_dimensionless_unit<ScalarUnit>::value,
      "Type `ScalarUnit` must be a dimensionless unit derived from `unit_t`.");
  return dimensionless::scalar_t(std::expm1(x()));
}

/**
 * @ingroup UnitMath
 * @brief Compute logarithm plus one
 * @details Returns the natural logarithm of one plus x. For small magnitude
 *          values of x, logp1 may be more accurate than log(1+x).
 * @param[in] x Value whose logarithm is calculated. If the argument is less
 *              than -1, a domain error occurs.
 * @returns The natural logarithm of (1+x).
 */
template <class ScalarUnit>
dimensionless::scalar_t log1p(const ScalarUnit x) noexcept {
  static_assert(
      traits::is_dimensionless_unit<ScalarUnit>::value,
      "Type `ScalarUnit` must be a dimensionless unit derived from `unit_t`.");
  return dimensionless::scalar_t(std::log1p(x()));
}

/**
 * @ingroup UnitMath
 * @brief Compute binary logarithm
 * @details Returns the binary (base-2) logarithm of x.
 * @param[in] x Value whose logarithm is calculated. If the argument is
 *              negative, a domain error occurs.
 * @returns The binary logarithm of x: log2x.
 */
template <class ScalarUnit>
dimensionless::scalar_t log2(const ScalarUnit x) noexcept {
  static_assert(
      traits::is_dimensionless_unit<ScalarUnit>::value,
      "Type `ScalarUnit` must be a dimensionless unit derived from `unit_t`.");
  return dimensionless::scalar_t(std::log2(x()));
}

//----------------------------------
// POWER FUNCTIONS
//----------------------------------

/* pow is implemented earlier in the library since a lot of the unit definitions
 * depend on it */

/**
 * @ingroup UnitMath
 * @brief computes the square root of <i>value</i>
 * @details Only implemented for linear_scale units.
 * @param[in] value `unit_t` derived type to compute the square root of.
 * @returns new unit_t, whose units are the square root of value's.
 *          E.g. if values had units of `square_meter`, then the return type
 *          will have units of `meter`.
 * @note `sqrt` provides a _rational approximation_ of the square root of
 *       <i>value</i>. In some cases, _both_ the returned value _and_ conversion
 *       factor of the returned unit type may have errors no larger than
 *       `1e-10`.
 */
template <
    class UnitType,
    std::enable_if_t<units::traits::has_linear_scale<UnitType>::value, int> = 0>
inline auto sqrt(const UnitType& value) noexcept -> unit_t<
    square_root<typename units::traits::unit_t_traits<UnitType>::unit_type>,
    typename units::traits::unit_t_traits<UnitType>::underlying_type,
    linear_scale> {
  return unit_t<
      square_root<typename units::traits::unit_t_traits<UnitType>::unit_type>,
      typename units::traits::unit_t_traits<UnitType>::underlying_type,
      linear_scale>(std::sqrt(value()));
}

/**
 * @ingroup UnitMath
 * @brief Computes the square root of the sum-of-squares of x and y.
 * @details Only implemented for linear_scale units.
 * @param[in] x unit_t type value
 * @param[in] y unit_t type value
 * @returns square root of the sum-of-squares of x and y in the same units as x.
 */
template <class UnitTypeLhs, class UnitTypeRhs,
          std::enable_if_t<
              units::traits::has_linear_scale<UnitTypeLhs, UnitTypeRhs>::value,
              int> = 0>
inline UnitTypeLhs hypot(const UnitTypeLhs& x, const UnitTypeRhs& y) {
  static_assert(traits::is_convertible_unit_t<UnitTypeLhs, UnitTypeRhs>::value,
                "Parameters of hypot() function are not compatible units.");
  return UnitTypeLhs(std::hypot(
      x(),
      y.template convert<
          typename units::traits::unit_t_traits<UnitTypeLhs>::unit_type>()()));
}

//----------------------------------
// ROUNDING FUNCTIONS
//----------------------------------

/**
 * @ingroup UnitMath
 * @brief Round up value
 * @details Rounds x upward, returning the smallest integral value that is not
 *          less than x.
 * @param[in] x Unit value to round up.
 * @returns The smallest integral value that is not less than x.
 */
template <class UnitType,
          class = std::enable_if_t<traits::is_unit_t<UnitType>::value>>
UnitType ceil(const UnitType x) noexcept {
  return UnitType(std::ceil(x()));
}

/**
 * @ingroup UnitMath
 * @brief Round down value
 * @details Rounds x downward, returning the largest integral value that is not
 *          greater than x.
 * @param[in] x Unit value to round down.
 * @returns The value of x rounded downward.
 */
template <class UnitType,
          class = std::enable_if_t<traits::is_unit_t<UnitType>::value>>
UnitType floor(const UnitType x) noexcept {
  return UnitType(std::floor(x()));
}

/**
 * @ingroup UnitMath
 * @brief Compute remainder of division
 * @details Returns the floating-point remainder of numer/denom (rounded towards
 *          zero).
 * @param[in] numer Value of the quotient numerator.
 * @param[in] denom Value of the quotient denominator.
 * @returns The remainder of dividing the arguments.
 */
template <class UnitTypeLhs, class UnitTypeRhs,
          class = std::enable_if_t<traits::is_unit_t<UnitTypeLhs>::value &&
                                   traits::is_unit_t<UnitTypeRhs>::value>>
UnitTypeLhs fmod(const UnitTypeLhs numer, const UnitTypeRhs denom) noexcept {
  static_assert(traits::is_convertible_unit_t<UnitTypeLhs, UnitTypeRhs>::value,
                "Parameters of fmod() function are not compatible units.");
  return UnitTypeLhs(std::fmod(
      numer(),
      denom.template convert<
          typename units::traits::unit_t_traits<UnitTypeLhs>::unit_type>()()));
}

/**
 * @ingroup UnitMath
 * @brief Truncate value
 * @details Rounds x toward zero, returning the nearest integral value that is
 *          not larger in magnitude than x. Effectively rounds towards 0.
 * @param[in] x Value to truncate
 * @returns The nearest integral value that is not larger in magnitude than x.
 */
template <class UnitType,
          class = std::enable_if_t<traits::is_unit_t<UnitType>::value>>
UnitType trunc(const UnitType x) noexcept {
  return UnitType(std::trunc(x()));
}

/**
 * @ingroup UnitMath
 * @brief Round to nearest
 * @details Returns the integral value that is nearest to x, with halfway cases
 *          rounded away from zero.
 * @param[in] x value to round.
 * @returns The value of x rounded to the nearest integral.
 */
template <class UnitType,
          class = std::enable_if_t<traits::is_unit_t<UnitType>::value>>
UnitType round(const UnitType x) noexcept {
  return UnitType(std::round(x()));
}

//----------------------------------
// FLOATING POINT MANIPULATION
//----------------------------------

/**
 * @ingroup UnitMath
 * @brief Copy sign
 * @details Returns a value with the magnitude and dimension of x, and the sign
 *          of y. Values x and y do not have to be compatible units.
 * @param[in] x Value with the magnitude of the resulting value.
 * @param[in] y Value with the sign of the resulting value.
 * @returns value with the magnitude and dimension of x, and the sign of y.
 */
template <class UnitTypeLhs, class UnitTypeRhs,
          class = std::enable_if_t<traits::is_unit_t<UnitTypeLhs>::value &&
                                   traits::is_unit_t<UnitTypeRhs>::value>>
UnitTypeLhs copysign(const UnitTypeLhs x, const UnitTypeRhs y) noexcept {
  return UnitTypeLhs(std::copysign(
      x(), y()));  // no need for conversion to get the correct sign.
}

/// Overload to copy the sign from a raw double
template <class UnitTypeLhs,
          class = std::enable_if_t<traits::is_unit_t<UnitTypeLhs>::value>>
UnitTypeLhs copysign(const UnitTypeLhs x,
                     const UNIT_LIB_DEFAULT_TYPE y) noexcept {
  return UnitTypeLhs(std::copysign(x(), y));
}

//----------------------------------
// MIN / MAX / DIFFERENCE
//----------------------------------

/**
 * @ingroup UnitMath
 * @brief Positive difference
 * @details The function returns x-y if x>y, and zero otherwise, in the same
 *          units as x. Values x and y do not have to be the same type of units,
 *          but they do have to be compatible.
 * @param[in] x Values whose difference is calculated.
 * @param[in] y Values whose difference is calculated.
 * @returns The positive difference between x and y.
 */
template <class UnitTypeLhs, class UnitTypeRhs,
          class = std::enable_if_t<traits::is_unit_t<UnitTypeLhs>::value &&
                                   traits::is_unit_t<UnitTypeRhs>::value>>
UnitTypeLhs fdim(const UnitTypeLhs x, const UnitTypeRhs y) noexcept {
  static_assert(traits::is_convertible_unit_t<UnitTypeLhs, UnitTypeRhs>::value,
                "Parameters of fdim() function are not compatible units.");
  return UnitTypeLhs(std::fdim(
      x(),
      y.template convert<
          typename units::traits::unit_t_traits<UnitTypeLhs>::unit_type>()()));
}

/**
 * @ingroup UnitMath
 * @brief Maximum value
 * @details Returns the larger of its arguments: either x or y, in the same
 *          units as x. Values x and y do not have to be the same type of units,
 *          but they do have to be compatible.
 * @param[in] x Values among which the function selects a maximum.
 * @param[in] y Values among which the function selects a maximum.
 * @returns The maximum numeric value of its arguments.
 */
template <class UnitTypeLhs, class UnitTypeRhs,
          class = std::enable_if_t<traits::is_unit_t<UnitTypeLhs>::value &&
                                   traits::is_unit_t<UnitTypeRhs>::value>>
UnitTypeLhs fmax(const UnitTypeLhs x, const UnitTypeRhs y) noexcept {
  static_assert(traits::is_convertible_unit_t<UnitTypeLhs, UnitTypeRhs>::value,
                "Parameters of fmax() function are not compatible units.");
  return UnitTypeLhs(std::fmax(
      x(),
      y.template convert<
          typename units::traits::unit_t_traits<UnitTypeLhs>::unit_type>()()));
}

/**
 * @ingroup UnitMath
 * @brief Minimum value
 * @details Returns the smaller of its arguments: either x or y, in the same
 *          units as x. If one of the arguments in a NaN, the other is returned.
 *          Values x and y do not have to be the same type of units, but they do
 *          have to be compatible.
 * @param[in] x Values among which the function selects a minimum.
 * @param[in] y Values among which the function selects a minimum.
 * @returns The minimum numeric value of its arguments.
 */
template <class UnitTypeLhs, class UnitTypeRhs,
          class = std::enable_if_t<traits::is_unit_t<UnitTypeLhs>::value &&
                                   traits::is_unit_t<UnitTypeRhs>::value>>
UnitTypeLhs fmin(const UnitTypeLhs x, const UnitTypeRhs y) noexcept {
  static_assert(traits::is_convertible_unit_t<UnitTypeLhs, UnitTypeRhs>::value,
                "Parameters of fmin() function are not compatible units.");
  return UnitTypeLhs(std::fmin(
      x(),
      y.template convert<
          typename units::traits::unit_t_traits<UnitTypeLhs>::unit_type>()()));
}

//----------------------------------
// OTHER FUNCTIONS
//----------------------------------

/**
 * @ingroup UnitMath
 * @brief Compute absolute value
 * @details Returns the absolute value of x, i.e. |x|.
 * @param[in] x Value whose absolute value is returned.
 * @returns The absolute value of x.
 */
template <class UnitType,
          class = std::enable_if_t<traits::is_unit_t<UnitType>::value>>
UnitType fabs(const UnitType x) noexcept {
  return UnitType(std::fabs(x()));
}

/**
 * @ingroup UnitMath
 * @brief Compute absolute value
 * @details Returns the absolute value of x, i.e. |x|.
 * @param[in] x Value whose absolute value is returned.
 * @returns The absolute value of x.
 */
template <class UnitType,
          class = std::enable_if_t<traits::is_unit_t<UnitType>::value>>
UnitType abs(const UnitType x) noexcept {
  return UnitType(std::fabs(x()));
}

/**
 * @ingroup UnitMath
 * @brief Multiply-add
 * @details Returns x*y+z. The function computes the result without losing
 *          precision in any intermediate result. The resulting unit type is a
 *          compound unit of x* y.
 * @param[in] x Values to be multiplied.
 * @param[in] y Values to be multiplied.
 * @param[in] z Value to be added.
 * @returns The result of x*y+z
 */
template <class UnitTypeLhs, class UnitMultiply, class UnitAdd,
          class = std::enable_if_t<traits::is_unit_t<UnitTypeLhs>::value &&
                                   traits::is_unit_t<UnitMultiply>::value &&
                                   traits::is_unit_t<UnitAdd>::value>>
auto fma(const UnitTypeLhs x, const UnitMultiply y, const UnitAdd z) noexcept
    -> decltype(x * y) {
  using resultType = decltype(x * y);
  static_assert(
      traits::is_convertible_unit_t<
          compound_unit<
              typename units::traits::unit_t_traits<UnitTypeLhs>::unit_type,
              typename units::traits::unit_t_traits<UnitMultiply>::unit_type>,
          typename units::traits::unit_t_traits<UnitAdd>::unit_type>::value,
      "Unit types are not compatible.");
  return resultType(std::fma(x(), y(), resultType(z)()));
}
}  // namespace units::math
