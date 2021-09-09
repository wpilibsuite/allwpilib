// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/sendable/Sendable.h>

namespace nt {

class NTSendableBuilder;

/**
 * Interface for NetworkTable Sendable objects.
 *
 * Objects add themselves to SendableRegistry. This class takes care of properly
 * calling Move() and Remove() on move and destruction. No action is taken if
 * the object is copied.
 */
class NTSendable : public wpi::Sendable {
 public:
  NTSendable() = default;

  NTSendable(const NTSendable& rhs) = default;
  NTSendable& operator=(const NTSendable& rhs) = default;

  NTSendable(NTSendable&& rhs) = default;
  NTSendable& operator=(NTSendable&& rhs) = default;

  /**
   * Initializes this Sendable object.
   *
   * @param builder sendable builder
   */
  virtual void InitSendable(NTSendableBuilder& builder) = 0;

  void InitSendable(wpi::SendableBuilder& builder) override;
};

}  // namespace nt
