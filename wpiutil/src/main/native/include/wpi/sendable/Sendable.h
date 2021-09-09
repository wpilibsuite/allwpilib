// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace wpi {

class SendableBuilder;

/**
 * Interface for Sendable objects.
 *
 * Objects add themselves to SendableRegistry. This class takes care of properly
 * calling Move() and Remove() on move and destruction. No action is taken if
 * the object is copied.
 */
class Sendable {
 public:
  Sendable() = default;

  Sendable(const Sendable& rhs) = default;
  Sendable& operator=(const Sendable& rhs) = default;

  Sendable(Sendable&& rhs);
  Sendable& operator=(Sendable&& rhs);

  virtual ~Sendable();

  /**
   * Initializes this Sendable object.
   *
   * @param builder sendable builder
   */
  virtual void InitSendable(SendableBuilder& builder) = 0;
};

}  // namespace wpi
