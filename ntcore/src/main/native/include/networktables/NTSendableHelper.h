// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>
#include <string_view>

#include <wpi/deprecated.h>
#include <wpi/sendable/SendableRegistry.h>

#include "networktables/NTSendable.h"

namespace nt {

/**
 * A helper class for use with objects that add themselves to SendableRegistry.
 * It takes care of properly calling Move() and Remove() on move and
 * destruction.  No action is taken if the object is copied.
 * Use public inheritance with CRTP when using this class.
 * @tparam CRTP derived class
 */
template <typename Derived>
class NTSendableHelper : public NTSendable {
 public:
  NTSendableHelper(const NTSendableHelper& rhs) = default;
  NTSendableHelper& operator=(const NTSendableHelper& rhs) = default;

  NTSendableHelper(NTSendableHelper&& rhs) {
    // it is safe to call Move() multiple times with the same rhs
    wpi::SendableRegistry::Move(this, &rhs);
  }

  NTSendableHelper& operator=(NTSendableHelper&& rhs) {
    // it is safe to call Move() multiple times with the same rhs
    wpi::SendableRegistry::Move(this, &rhs);
    return *this;
  }

 protected:
  NTSendableHelper() = default;

  ~NTSendableHelper() override {
    // it is safe to call Remove() multiple times with the same object
    wpi::SendableRegistry::Remove(this);
  }
};

}  // namespace nt
