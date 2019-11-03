/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <wpi/StringRef.h>

#include "frc/smartdashboard/Sendable.h"
#include "frc/smartdashboard/SendableBuilder.h"
#include "frc/smartdashboard/SendableHelper.h"

namespace frc {

/**
 * A mock sendable that marks itself as an actuator.
 */
class MockActuatorSendable : public Sendable,
                             public SendableHelper<MockActuatorSendable> {
 public:
  explicit MockActuatorSendable(wpi::StringRef name);

  void InitSendable(SendableBuilder& builder) override;
};

}  // namespace frc
