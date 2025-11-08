// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/sendable/Sendable.h>

namespace nt {

class NTSendableBuilder;

/**
 * Interface for NetworkTable Sendable objects.
 */
class NTSendable : public wpi::Sendable {
 public:
  /**
   * Initializes this Sendable object.
   *
   * @param builder sendable builder
   */
  virtual void InitSendable(NTSendableBuilder& builder) = 0;

  void InitSendable(wpi::SendableBuilder& builder) override;
};

}  // namespace nt
