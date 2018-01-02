/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#if !defined(__clang__) && defined(__GNUC__) && __GNUC__ < 5
static_assert(0,
              "GCC must be 5 or greater. If building for the roboRIO, please "
              "update to the 2018 toolchains.");
#endif

#if defined(_MSC_VER) && _MSC_VER < 1900
static_assert(0, "Visual Studio 2015 or greater required.");
#endif

#include <HAL/cpp/make_unique.h>

#define DEFAULT_MOVE_CONSTRUCTOR(ClassName) ClassName(ClassName&&) = default

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
