/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "frc/ErrorBase.h"
#include "frc/interfaces/Accelerometer.h"
#include "frc/smartdashboard/Sendable.h"
#include "frc/smartdashboard/SendableHelper.h"

namespace frc {

class SendableBuilder;

/**
 * Built-in accelerometer.
 *
 * This class allows access to the roboRIO's internal accelerometer.
 */
class BuiltInAccelerometer : public ErrorBase,
                             public Accelerometer,
                             public Sendable,
                             public SendableHelper<BuiltInAccelerometer> {
 public:
  /**
   * Constructor.
   *
   * @param range The range the accelerometer will measure
   */
  explicit BuiltInAccelerometer(Range range = kRange_8G);

  BuiltInAccelerometer(BuiltInAccelerometer&&) = default;
  BuiltInAccelerometer& operator=(BuiltInAccelerometer&&) = default;

  // Accelerometer interface
  /**
   * Set the measuring range of the accelerometer.
   *
   * @param range The maximum acceleration, positive or negative, that the
   *              accelerometer will measure. Not all accelerometers support all
   *              ranges.
   */
  void SetRange(Range range) override;

  /**
   * @return The acceleration of the roboRIO along the X axis in g-forces
   */
  double GetX() override;

  /**
   * @return The acceleration of the roboRIO along the Y axis in g-forces
   */
  double GetY() override;

  /**
   * @return The acceleration of the roboRIO along the Z axis in g-forces
   */
  double GetZ() override;

  void InitSendable(SendableBuilder& builder) override;
};

}  // namespace frc
