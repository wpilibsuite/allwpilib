// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

#include "frc/interfaces/Accelerometer.h"

namespace frc {

/**
 * Built-in accelerometer.
 *
 * This class allows access to the roboRIO's internal accelerometer.
 */
class BuiltInAccelerometer : public Accelerometer,
                             public wpi::Sendable,
                             public wpi::SendableHelper<BuiltInAccelerometer> {
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
  void SetRange(Range range) final;

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

  void InitSendable(wpi::SendableBuilder& builder) override;
};

}  // namespace frc
