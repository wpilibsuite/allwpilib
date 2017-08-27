/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <HAL/cpp/make_unique.h>

// MSVC 2013 doesn't allow "= default" on move constructors, but since we are
// (currently) only actually using the move constructors in non-MSVC situations
// (ie, wpilibC++Devices), we can just ignore it in MSVC.
#if defined(_MSC_VER) && _MSC_VER < 1900
#define DEFAULT_MOVE_CONSTRUCTOR(ClassName)
#else
#define DEFAULT_MOVE_CONSTRUCTOR(ClassName) ClassName(ClassName&&) = default
#endif

#if defined(_MSC_VER) && _MSC_VER < 1900
#define constexpr const
#endif

#if (__cplusplus < 201103L) && !defined(_MSC_VER)
#define nullptr NULL
#endif

#if defined(_MSC_VER) && _MSC_VER < 1900
#define noexcept throw()
#endif

// Provide std::decay_t when using GCC < 4.9
#if defined(__GNUC__)
#if __GNUC__ == 4 && __GNUC_MINOR__ < 9
#include <type_traits>
namespace std {
template <class T>
using decay_t = typename decay<T>::type;
}
#endif
#endif

namespace frc {

// A struct to use as a deleter when a std::shared_ptr must wrap a raw pointer
// that is being deleted by someone else.
template <class T>
struct NullDeleter {
  void operator()(T*) const noexcept {};
};

}  // namespace frc

#include <atomic>

namespace frc {

// Use this for determining whether the default move constructor has been
// called on a containing object. This serves the purpose of allowing us to
// use the default move constructor of an object for moving all the data around
// while being able to use this to, for instance, chose not to de-allocate
// a PWM port in a destructor.
struct HasBeenMoved {
  HasBeenMoved(HasBeenMoved&& other) {
    other.moved = true;
    moved = false;
  }
  HasBeenMoved() = default;
  std::atomic<bool> moved{false};
  operator bool() const { return moved; }
};

}  // namespace frc

// For backwards compatibility
#ifndef NAMESPACED_WPILIB
using namespace frc;  // NOLINT
#endif
