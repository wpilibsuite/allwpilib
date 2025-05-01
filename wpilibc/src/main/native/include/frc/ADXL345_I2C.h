// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <hal/SimDevice.h>
#include <networktables/NTSendable.h>
#include <wpi/sendable/SendableHelper.h>

#include "frc/I2C.h"

namespace frc {

/**
 * ADXL345 Accelerometer on I2C.
 *
 * This class allows access to a Analog Devices ADXL345 3-axis accelerometer on
 * an I2C bus. This class assumes the default (not alternate) sensor address of
 * 0x1D (7-bit address).
 */
class ADXL345_I2C : public nt::NTSendable,
                    public wpi::SendableHelper<ADXL345_I2C> {
 public:
  /**
   * Accelerometer range.
   */
  enum Range {
    /// 2 Gs max.
    RANGE_2G = 0,
    /// 4 Gs max.
    RANGE_4G = 1,
    /// 8 Gs max.
    RANGE_8G = 2,
    /// 16 Gs max.
    RANGE_16G = 3
  };

  /**
   * Accelerometer axes.
   */
  enum Axes {
    /// X axis.
    AXIS_X = 0x00,
    /// Y axis.
    AXIS_Y = 0x02,
    /// Z axis.
    AXIS_Z = 0x04
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

  /// Default I2C device address.
  static constexpr int ADDRESS = 0x1D;

  /**
   * Constructs the ADXL345 Accelerometer over I2C.
   *
   * @param port          The I2C port the accelerometer is attached to
   * @param range         The range (+ or -) that the accelerometer will measure
   * @param deviceAddress The I2C address of the accelerometer (0x1D or 0x53)
   */
  explicit ADXL345_I2C(I2C::Port port, Range range = RANGE_2G,
                       int deviceAddress = ADDRESS);
  ~ADXL345_I2C() override = default;

  ADXL345_I2C(ADXL345_I2C&&) = default;
  ADXL345_I2C& operator=(ADXL345_I2C&&) = default;

  I2C::Port GetI2CPort() const;
  int GetI2CDeviceAddress() const;

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
   * @return Acceleration of the ADXL345 in Gs.
   */
  virtual double GetAcceleration(Axes axis);

  /**
   * Get the acceleration of all axes in Gs.
   *
   * @return An object containing the acceleration measured on each axis of the
   *         ADXL345 in Gs.
   */
  virtual AllAxes GetAccelerations();

  void InitSendable(nt::NTSendableBuilder& builder) override;

 private:
  I2C m_i2c;

  hal::SimDevice m_simDevice;
  hal::SimEnum m_simRange;
  hal::SimDouble m_simX;
  hal::SimDouble m_simY;
  hal::SimDouble m_simZ;

  static constexpr int POWER_CTL_REGISTER = 0x2D;
  static constexpr int DATA_FORMAT_REGISTER = 0x31;
  static constexpr int DATA_REGISTER = 0x32;
  static constexpr double GS_PER_LSB = 0.00390625;

  enum PowerCtlFields {
    POWER_CTL_LINK = 0x20,
    POWER_CTL_AUTO_SLEEP = 0x10,
    POWER_CTL_MEASURE = 0x08,
    POWER_CTL_SLEEP = 0x04
  };

  enum DataFormatFields {
    DATA_FORMAT_SELF_TEST = 0x80,
    DATA_FORMAT_SPI = 0x40,
    DATA_FORMAT_INT_INVERT = 0x20,
    DATA_FORMAT_FULL_RES = 0x08,
    DATA_FORMAT_JUSTIFY = 0x04
  };
};

}  // namespace frc
