// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace frc {

/**
 * Interface for 3-axis accelerometers.
 *
 * @deprecated This interface is being removed with no replacement.
 */
class [[deprecated(
    "This interface is being removed with no replacement.")]] Accelerometer {
 public:
  Accelerometer() = default;
  virtual ~Accelerometer() = default;

  Accelerometer(Accelerometer&&) = default;
  Accelerometer& operator=(Accelerometer&&) = default;

  /**
   * Accelerometer range.
   */
  enum Range {
    /**
     * 2 Gs max.
     */
    kRange_2G = 0,
    /**
     * 4 Gs max.
     */
    kRange_4G = 1,
    /**
     * 8 Gs max.
     */
    kRange_8G = 2,
    /**
     * 16 Gs max.
     */
    kRange_16G = 3
  };

  /**
   * Common interface for setting the measuring range of an accelerometer.
   *
   * @param range The maximum acceleration, positive or negative, that the
   *              accelerometer will measure. Not all accelerometers support all
   *              ranges.
   */
  virtual void SetRange(Range range) = 0;

  /**
   * Common interface for getting the x axis acceleration.
   *
   * @return The acceleration along the x axis in g-forces
   */
  virtual double GetX() = 0;

  /**
   * Common interface for getting the y axis acceleration.
   *
   * @return The acceleration along the y axis in g-forces
   */
  virtual double GetY() = 0;

  /**
   * Common interface for getting the z axis acceleration.
   *
   * @return The acceleration along the z axis in g-forces
   */
  virtual double GetZ() = 0;
};

}  // namespace frc
