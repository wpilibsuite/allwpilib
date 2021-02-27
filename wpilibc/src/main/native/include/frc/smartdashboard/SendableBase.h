// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/deprecated.h>

#include "frc/smartdashboard/Sendable.h"
#include "frc/smartdashboard/SendableHelper.h"

namespace frc {

class SendableBase : public Sendable, public SendableHelper<SendableBase> {
 public:
  /**
   * Creates an instance of the sensor base
   *
   * @deprecated use Sendable and SendableHelper
   *
   * @param addLiveWindow if true, add this Sendable to LiveWindow
   */
  WPI_DEPRECATED("use Sendable and SendableHelper")
  explicit SendableBase(bool addLiveWindow = true);
};

}  // namespace frc
