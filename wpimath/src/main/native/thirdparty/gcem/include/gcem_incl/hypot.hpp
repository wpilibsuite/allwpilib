/*################################################################################
  ##
  ##   Copyright (C) 2016-2024 Keith O'Hara
  ##
  ##   This file is part of the GCE-Math C++ library.
  ##
  ##   Licensed under the Apache License, Version 2.0 (the "License");
  ##   you may not use this file except in compliance with the License.
  ##   You may obtain a copy of the License at
  ##
  ##       http://www.apache.org/licenses/LICENSE-2.0
  ##
  ##   Unless required by applicable law or agreed to in writing, software
  ##   distributed under the License is distributed on an "AS IS" BASIS,
  ##   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  ##   See the License for the specific language governing permissions and
  ##   limitations under the License.
  ##
  ################################################################################*/

/*
 * compile-time Pythagorean addition function
 */

// see: https://en.wikipedia.org/wiki/Pythagorean_addition

#ifndef _gcem_hypot_HPP
#define _gcem_hypot_HPP

#include <algorithm>
#include <cmath>
#include <type_traits>

namespace gcem
{

namespace internal
{

template<typename T>
constexpr
T
hypot_compute(const T x, const T y)
noexcept
{
    T a = std::max(abs(x), abs(y));
    if (a) {
      return a * sqrt((x / a) * (x / a) + (y / a) * (y / a));
    } else {
      return {};
    }
}

template<typename T>
constexpr
T
hypot_compute(const T x, const T y, const T z)
noexcept
{
    T a = std::max({abs(x), abs(y), abs(z)});
    if (a) {
      return a * sqrt((x / a) * (x / a) + (y / a) * (y / a) + (z / a) * (z / a));
    } else {
      return {};
    }
}

template<typename T>
constexpr
T
hypot_vals_check(const T x, const T y)
noexcept
{
    return( any_nan(x, y) ? \
                GCLIM<T>::quiet_NaN() :
            //
            any_inf(x,y) ? \
                GCLIM<T>::infinity() :
            // indistinguishable from zero or one
            GCLIM<T>::min() > abs(x) ? \
                abs(y) :
            GCLIM<T>::min() > abs(y) ? \
                abs(x) :
            // else
            hypot_compute(x, y) );
}

template<typename T>
constexpr
T
hypot_vals_check(const T x, const T y, const T z)
noexcept
{
    return( any_nan(x, y, z) ? \
                GCLIM<T>::quiet_NaN() :
            //
            any_inf(x,y,z) ? \
                GCLIM<T>::infinity() :
            // indistinguishable from zero or one
            GCLIM<T>::min() > abs(x) && GCLIM<T>::min() > abs(y) ? \
                abs(z) :
            GCLIM<T>::min() > abs(x) && GCLIM<T>::min() > abs(z) ? \
                abs(y) :
            GCLIM<T>::min() > abs(y) && GCLIM<T>::min() > abs(z) ? \
                abs(x) :
            GCLIM<T>::min() > abs(x) ? \
                hypot_vals_check(y, z) :
            GCLIM<T>::min() > abs(y) ? \
                hypot_vals_check(x, z) :
            GCLIM<T>::min() > abs(z) ? \
                hypot_vals_check(x, y) :
            // else
            hypot_compute(x, y, z) );
}

template<typename T1, typename T2, typename TC = common_return_t<T1,T2>>
constexpr
TC
hypot_type_check(const T1 x, const T2 y)
noexcept
{
    return hypot_vals_check(static_cast<TC>(x),static_cast<TC>(y));
}

template<typename T1, typename T2, typename T3, typename TC = common_return_t<T1,T2,T3>>
constexpr
TC
hypot_type_check(const T1 x, const T2 y, const T3 z)
noexcept
{
    return hypot_vals_check(static_cast<TC>(x),static_cast<TC>(y),static_cast<TC>(z));
}

}

/**
 * Compile-time Pythagorean addition function
 *
 * @param x a real-valued input.
 * @param y a real-valued input.
 * @return Computes \f$ x \oplus y = \sqrt{x^2 + y^2} \f$.
 */

template<typename T1, typename T2>
constexpr
common_return_t<T1,T2>
hypot(const T1 x, const T2 y)
noexcept
{
  if (std::is_constant_evaluated()) {
    return internal::hypot_type_check(x,y);
  } else {
    return std::hypot(x, y);
  }
}

/**
 * Compile-time Pythagorean addition function
 *
 * @param x a real-valued input.
 * @param y a real-valued input.
 * @param z a real-valued input.
 * @return Computes \f$ x \oplus y \oplus z = \sqrt{x^2 + y^2 + z^2} \f$.
 */

template<typename T1, typename T2, typename T3>
constexpr
common_return_t<T1,T2,T3>
hypot(const T1 x, const T2 y, const T3 z)
noexcept
{
  if (std::is_constant_evaluated()) {
    return internal::hypot_type_check(x,y,z);
  } else {
    return std::hypot(x, y, z);
  }
}

}

#endif
