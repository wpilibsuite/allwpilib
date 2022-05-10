//===- llvm/Support/type_traits.h - Simplfied type traits -------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file provides useful additions to the standard type_traits library.
//
//===----------------------------------------------------------------------===//

#ifndef WPIUTIL_WPI_TYPE_TRAITS_H
#define WPIUTIL_WPI_TYPE_TRAITS_H

#include "wpi/Compiler.h"
#include <type_traits>
#include <utility>

#ifndef __has_feature
#define WPI_DEFINED_HAS_FEATURE
#define __has_feature(x) 0
#endif

namespace wpi {


/// Metafunction that determines whether the given type is either an
/// integral type or an enumeration type, including enum classes.
///
/// Note that this accepts potentially more integral types than is_integral
/// because it is based on being implicitly convertible to an integral type.
/// Also note that enum classes aren't implicitly convertible to integral types,
/// the value may therefore need to be explicitly converted before being used.
template <typename T> class is_integral_or_enum {
  using UnderlyingT = typename std::remove_reference<T>::type;

public:
  static const bool value =
      !std::is_class<UnderlyingT>::value && // Filter conversion operators.
      !std::is_pointer<UnderlyingT>::value &&
      !std::is_floating_point<UnderlyingT>::value &&
      (std::is_enum<UnderlyingT>::value ||
       std::is_convertible<UnderlyingT, unsigned long long>::value);
};

/// If T is a pointer, just return it. If it is not, return T&.
template<typename T, typename Enable = void>
struct add_lvalue_reference_if_not_pointer { using type = T &; };

template <typename T>
struct add_lvalue_reference_if_not_pointer<
    T, typename std::enable_if<std::is_pointer<T>::value>::type> {
  using type = T;
};

/// If T is a pointer to X, return a pointer to const X. If it is not,
/// return const T.
template<typename T, typename Enable = void>
struct add_const_past_pointer { using type = const T; };

template <typename T>
struct add_const_past_pointer<
    T, typename std::enable_if<std::is_pointer<T>::value>::type> {
  using type = const typename std::remove_pointer<T>::type *;
};

template <typename T, typename Enable = void>
struct const_pointer_or_const_ref {
  using type = const T &;
};
template <typename T>
struct const_pointer_or_const_ref<
    T, typename std::enable_if<std::is_pointer<T>::value>::type> {
  using type = typename add_const_past_pointer<T>::type;
};

} // end namespace wpi

// If the compiler supports detecting whether a class is final, define
// an LLVM_IS_FINAL macro. If it cannot be defined properly, this
// macro will be left undefined.
#ifndef LLVM_IS_FINAL
#if __cplusplus >= 201402L || defined(_MSC_VER)
#define LLVM_IS_FINAL(Ty) std::is_final<Ty>()
#elif __has_feature(is_final) || LLVM_GNUC_PREREQ(4, 7, 0)
#define LLVM_IS_FINAL(Ty) __is_final(Ty)
#endif
#endif

#ifdef WPI_DEFINED_HAS_FEATURE
#undef __has_feature
#undef WPI_DEFINED_HAS_FEATURE
#endif

#endif // WPIUTIL_WPI_TYPE_TRAITS_H
