/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "frc/SpeedController.h"
#include "frc/smartdashboard/Sendable.h"

namespace frc {

/**
 * Interface for speed controlling devices that can be displayed on a dashboard.
 */
class SendableSpeedController : public Sendable, public SpeedController {
 public:
  ~SendableSpeedController() override = default;

  void InitSendable(SendableBuilder& builder) override;
};

}  // namespace frc
