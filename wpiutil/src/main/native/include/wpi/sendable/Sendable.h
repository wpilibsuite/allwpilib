// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/SymbolExports.h"

namespace wpi {

class SendableBuilder;

/**
 * Interface for Sendable objects.
 */
class WPILIB_DLLEXPORT Sendable {
 public:
  constexpr virtual ~Sendable() = default;

  /**
   * Initializes this Sendable object.
   *
   * @param builder sendable builder
   */
  virtual void InitSendable(SendableBuilder& builder) = 0;
};

}  // namespace wpi
