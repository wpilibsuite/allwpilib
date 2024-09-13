// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <hal/SimDevice.h>
#include <networktables/NTSendable.h>
#include <wpi/sendable/SendableHelper.h>

#include "frc/SPI.h"

namespace frc {

/**
 * ADXL362 SPI Accelerometer.
 *
 * This class allows access to an Analog Devices ADXL362 3-axis accelerometer.
 */
class ADXL362 : public nt::NTSendable, public wpi::SendableHelper<ADXL362> {
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
   * Accelerometer axes.
   */
  enum Axes {
    /// X axis.
    kAxis_X = 0x00,
    /// Y axis.
    kAxis_Y = 0x02,
    /// Z axis.
    kAxis_Z = 0x04
  };

  /**
   * Container type for accelerations from all axes.
   */
  struct AllAxes {
    /// Acceleration along the X axis in g-forces.
    double XAxis = 0.0;
    /// Acceleration along the Y axis in g-forces.
    double YAxis = 0.0;
    /// Acceleration along the Z axis in g-forces.
    double ZAxis = 0.0;
  };

 public:
  /**
   * Constructor.  Uses the onboard CS1.
   *
   * @param range The range (+ or -) that the accelerometer will measure.
   */
  explicit ADXL362(Range range = kRange_2G);

  /**
   * Constructor.
   *
   * @param port  The SPI port the accelerometer is attached to
   * @param range The range (+ or -) that the accelerometer will measure.
   */
  explicit ADXL362(SPI::Port port, Range range = kRange_2G);

  ~ADXL362() override = default;

  ADXL362(ADXL362&&) = default;
  ADXL362& operator=(ADXL362&&) = default;

  SPI::Port GetSpiPort() const;

  /**
   * Set the measuring range of the accelerometer.
   *
   * @param range The maximum acceleration, positive or negative, that the
   *     accelerometer will measure.
   */
  void SetRange(Range range);

  /**
   * Returns the acceleration along the X axis in g-forces.
   *
   * @return The acceleration along the X axis in g-forces.
   */
  double GetX();

  /**
   * Returns the acceleration along the Y axis in g-forces.
   *
   * @return The acceleration along the Y axis in g-forces.
   */
  double GetY();

  /**
   * Returns the acceleration along the Z axis in g-forces.
   *
   * @return The acceleration along the Z axis in g-forces.
   */
  double GetZ();

  /**
   * Get the acceleration of one axis in Gs.
   *
   * @param axis The axis to read from.
   * @return Acceleration of the ADXL362 in Gs.
   */
  virtual double GetAcceleration(Axes axis);

  /**
   * Get the acceleration of all axes in Gs.
   *
   * @return An object containing the acceleration measured on each axis of the
   *         ADXL362 in Gs.
   */
  virtual AllAxes GetAccelerations();

  void InitSendable(nt::NTSendableBuilder& builder) override;

 private:
  SPI m_spi;
  hal::SimDevice m_simDevice;
  hal::SimEnum m_simRange;
  hal::SimDouble m_simX;
  hal::SimDouble m_simY;
  hal::SimDouble m_simZ;
  double m_gsPerLSB = 0.001;
};

}  // namespace frc
