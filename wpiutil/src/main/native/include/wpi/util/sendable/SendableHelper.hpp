// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <type_traits>

#include "wpi/sendable/SendableRegistry.h"

namespace wpi {

/**
 * A helper class for use with objects that add themselves to SendableRegistry.
 * It takes care of properly calling Move() and Remove() on move and
 * destruction.  No action is taken if the object is copied.
 * Use public inheritance with CRTP when using this class.
 * @tparam CRTP derived class
 */
template <typename Derived>
class SendableHelper {
 public:
  constexpr SendableHelper(const SendableHelper& rhs) = default;
  constexpr SendableHelper& operator=(const SendableHelper& rhs) = default;

#if !defined(_MSC_VER) && (defined(__llvm__) || __GNUC__ > 7)
  // See https://bugzilla.mozilla.org/show_bug.cgi?id=1442819
  __attribute__((no_sanitize("vptr")))
#endif
  constexpr SendableHelper(SendableHelper&& rhs) {
    if (!std::is_constant_evaluated()) {
      // it is safe to call Move() multiple times with the same rhs
      SendableRegistry::Move(static_cast<Derived*>(this),
                             static_cast<Derived*>(&rhs));
    }
  }

#if !defined(_MSC_VER) && (defined(__llvm__) || __GNUC__ > 7)
  // See https://bugzilla.mozilla.org/show_bug.cgi?id=1442819
  __attribute__((no_sanitize("vptr")))
#endif
  constexpr SendableHelper& operator=(SendableHelper&& rhs) {
    if (!std::is_constant_evaluated()) {
      // it is safe to call Move() multiple times with the same rhs
      SendableRegistry::Move(static_cast<Derived*>(this),
                             static_cast<Derived*>(&rhs));
    }
    return *this;
  }

 protected:
  constexpr SendableHelper() = default;

  constexpr ~SendableHelper() {
    if (!std::is_constant_evaluated()) {
      // it is safe to call Remove() multiple times with the same object
      SendableRegistry::Remove(static_cast<Derived*>(this));
    }
  }
};

}  // namespace wpi
