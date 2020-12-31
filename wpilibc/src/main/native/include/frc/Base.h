// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#if !defined(__clang__) && defined(__GNUC__) && __GNUC__ < 5
static_assert(0,
              "GCC must be 5 or greater. If building for the roboRIO, please "
              "update to the 2018 toolchains.");
#endif

#if defined(_MSC_VER) && _MSC_VER < 1900
static_assert(0, "Visual Studio 2015 or greater required.");
#endif

/** WPILib FRC namespace */
namespace frc {

// A struct to use as a deleter when a std::shared_ptr must wrap a raw pointer
// that is being deleted by someone else.
template <class T>
struct NullDeleter {
  void operator()(T*) const noexcept {};
};

}  // namespace frc

// For backwards compatibility
#ifdef NO_NAMESPACED_WPILIB
using namespace frc;  // NOLINT
#endif
