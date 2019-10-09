/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <wpi/deprecated.h>

#include "frc/smartdashboard/Sendable.h"
#include "frc/smartdashboard/SendableHelper.h"

namespace frc {

class SendableBase : public Sendable, public SendableHelper<SendableBase> {
 public:
  /**
   * Creates an instance of the sensor base.
   *
   * @param addLiveWindow if true, add this Sendable to LiveWindow
   */
  WPI_DEPRECATED("use Sendable and SendableHelper")
  explicit SendableBase(bool addLiveWindow = true);
};

}  // namespace frc
