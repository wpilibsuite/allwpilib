// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/StringRef.h>

#include "frc/smartdashboard/SendableBase.h"
#include "frc/smartdashboard/SendableBuilder.h"

namespace frc {

/**
 * A mock sendable that marks itself as an actuator.
 */
class MockActuatorSendable : public SendableBase {
 public:
  explicit MockActuatorSendable(wpi::StringRef name);

  void InitSendable(SendableBuilder& builder) override;
};

}  // namespace frc
