/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
