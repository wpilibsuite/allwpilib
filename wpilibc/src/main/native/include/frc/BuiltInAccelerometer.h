// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

namespace frc {

/**
 * Built-in accelerometer.
 *
 * This class allows access to the roboRIO's internal accelerometer.
 */
class BuiltInAccelerometer : public wpi::Sendable,
                             public wpi::SendableHelper<BuiltInAccelerometer> {
 public:
  /**
   * Accelerometer range.
   */
  enum Range {
    /// 2 Gs max.
    kRange_2G = 0,
    /// 4 Gs max.
    kRange_4G = 1,
    /// 8 Gs max.
    kRange_8G = 2
  };

  /**
   * Constructor.
   *
   * @param range The range the accelerometer will measure
   */
  explicit BuiltInAccelerometer(Range range = kRange_8G);

  BuiltInAccelerometer(BuiltInAccelerometer&&) = default;
  BuiltInAccelerometer& operator=(BuiltInAccelerometer&&) = default;

  /**
   * Set the measuring range of the accelerometer.
   *
   * @param range The maximum acceleration, positive or negative, that the
   *     accelerometer will measure.
   */
  void SetRange(Range range);

  /**
   * @return The acceleration of the roboRIO along the X axis in g-forces
   */
  double GetX();

  /**
   * @return The acceleration of the roboRIO along the Y axis in g-forces
   */
  double GetY();

  /**
   * @return The acceleration of the roboRIO along the Z axis in g-forces
   */
  double GetZ();

  void InitSendable(wpi::SendableBuilder& builder) override;
};

}  // namespace frc
