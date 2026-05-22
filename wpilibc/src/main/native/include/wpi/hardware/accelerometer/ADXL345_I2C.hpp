// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/hal/SimDevice.hpp"
#include "wpi/hardware/bus/I2C.hpp"
#include "wpi/nt/NTSendable.hpp"
#include "wpi/util/sendable/SendableHelper.hpp"

namespace wpi {

/**
 * ADXL345 Accelerometer on I2C.
 *
 * This class allows access to a Analog Devices ADXL345 3-axis accelerometer on
 * an I2C bus. This class assumes the default (not alternate) sensor address of
 * 0x1D (7-bit address).
 */
class ADXL345_I2C : public wpi::nt::NTSendable,
                    public wpi::util::SendableHelper<ADXL345_I2C> {
 public:
  /**
   * Accelerometer axes.
   */
  enum class Axis {
    /// X axis.
    X = 0x00,
    /// Y axis.
    Y = 0x02,
    /// Z axis.
    Z = 0x04
  };

  /**
   * Container type for accelerations from all axes.
   */
  struct AllAxes {
    /// Acceleration along the X axis in g-forces.
    double x = 0.0;
    /// Acceleration along the Y axis in g-forces.
    double y = 0.0;
    /// Acceleration along the Z axis in g-forces.
    double z = 0.0;
  };

  /// Default I2C device address.
  static constexpr int DEFAULT_ADDRESS = 0x1D;

  /**
   * Constructs the ADXL345 Accelerometer over I2C.
   *
   * @param port          The I2C port the accelerometer is attached to
   * @param range         The range (+ or -) that the accelerometer will
   *                      measure; valid values are 2, 4, 8, or 16 Gs. The
   *                      default is 2 Gs.
   * @param deviceAddress The I2C address of the accelerometer (0x1D or 0x53)
   */
  explicit ADXL345_I2C(I2C::Port port, int range = 2,
                       int deviceAddress = DEFAULT_ADDRESS);
  ~ADXL345_I2C() override = default;

  ADXL345_I2C(ADXL345_I2C&&) = default;
  ADXL345_I2C& operator=(ADXL345_I2C&&) = default;

  I2C::Port GetI2CPort() const;
  int GetI2CDeviceAddress() const;

  /**
   * Set the measuring range of the accelerometer.
   *
   * @param range The maximum acceleration, positive or negative, that the
   *     accelerometer will measure. Valid values are 2, 4, 8, or 16 Gs.
   */
  void SetRange(int range);

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
  virtual double GetAcceleration(Axis axis);

  /**
   * Get the acceleration of all axes in Gs.
   *
   * @return An object containing the acceleration measured on each axis of the
   *         ADXL345 in Gs.
   */
  virtual AllAxes GetAccelerations();

  void InitSendable(wpi::nt::NTSendableBuilder& builder) override;

 private:
  I2C m_i2c;

  wpi::hal::SimDevice m_simDevice;
  wpi::hal::SimEnum m_simRange;
  wpi::hal::SimDouble m_simX;
  wpi::hal::SimDouble m_simY;
  wpi::hal::SimDouble m_simZ;
};

}  // namespace wpi
