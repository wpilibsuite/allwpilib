// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

//===- llvm/ADT/STLExtras.h - Useful STL related functions ------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef WPIUTIL_WPI_FUNCTION_REF_H_
#define WPIUTIL_WPI_FUNCTION_REF_H_

#include <stdint.h>

#include <type_traits>
#include <utility>

namespace wpi {

/// An efficient, type-erasing, non-owning reference to a callable. This is
/// intended for use as the type of a function parameter that is not used
/// after the function in question returns.
///
/// This class does not own the callable, so it is not in general safe to store
/// a function_ref.
template <typename Fn>
class function_ref;

template <typename Ret, typename... Params>
class function_ref<Ret(Params...)> {
  Ret (*callback)(intptr_t callable, Params... params) = nullptr;
  intptr_t callable;

  template <typename Callable>
  static Ret callback_fn(intptr_t callable, Params... params) {
    return (*reinterpret_cast<Callable*>(callable))(std::forward<Params>(
        params)...);
  }

 public:
  function_ref() = default;
  /*implicit*/ function_ref(std::nullptr_t) {}  // NOLINT

  template <typename Callable>
  /*implicit*/ function_ref(  // NOLINT
      Callable&& callable,
      typename std::enable_if<
          !std::is_same<typename std::remove_reference<Callable>::type,
                        function_ref>::value>::type* = nullptr)
      : callback(callback_fn<typename std::remove_reference<Callable>::type>),
        callable(reinterpret_cast<intptr_t>(&callable)) {}

  Ret operator()(Params... params) const {
    return callback(callable, std::forward<Params>(params)...);
  }

  explicit operator bool() const { return callback; }
};

}  // namespace wpi

#endif  // WPIUTIL_WPI_FUNCTION_REF_H_
