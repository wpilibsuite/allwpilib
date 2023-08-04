// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <hal/SimDevice.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

#include "frc/SPI.h"
#include "frc/geometry/Rotation2d.h"

namespace frc {

/**
 * Use a rate gyro to return the robots heading relative to a starting position.
 *
 * The %Gyro class tracks the robots heading based on the starting position. As
 * the robot rotates the new heading is computed by integrating the rate of
 * rotation returned by the sensor. When the class is instantiated, it does a
 * short calibration routine where it samples the gyro while at rest to
 * determine the default offset. This is subtracted from each sample to
 * determine the heading.
 *
 * This class is for the digital ADXRS450 gyro sensor that connects via SPI.
 * Only one instance of an ADXRS %Gyro is supported.
 */
class ADXRS450_Gyro : public wpi::Sendable,
                      public wpi::SendableHelper<ADXRS450_Gyro> {
 public:
  /**
   * %Gyro constructor on onboard CS0.
   */
  ADXRS450_Gyro();

  /**
   * %Gyro constructor on the specified SPI port.
   *
   * @param port The SPI port the gyro is attached to.
   */
  explicit ADXRS450_Gyro(SPI::Port port);

  ~ADXRS450_Gyro() override = default;

  ADXRS450_Gyro(ADXRS450_Gyro&&) = default;
  ADXRS450_Gyro& operator=(ADXRS450_Gyro&&) = default;

  bool IsConnected() const;

  /**
   * Return the actual angle in degrees that the robot is currently facing.
   *
   * The angle is based on integration of the returned rate from the gyro.
   * The angle is continuous, that is it will continue from 360->361 degrees.
   * This allows algorithms that wouldn't want to see a discontinuity in the
   * gyro output as it sweeps from 360 to 0 on the second time around.
   *
   * @return the current heading of the robot in degrees.
   */
  double GetAngle() const;

  /**
   * Return the rate of rotation of the gyro
   *
   * The rate is based on the most recent reading of the gyro.
   *
   * @return the current rate in degrees per second
   */
  double GetRate() const;

  /**
   * Reset the gyro.
   *
   * Resets the gyro to a heading of zero. This can be used if there is
   * significant drift in the gyro and it needs to be recalibrated after it has
   * been running.
   */
  void Reset();

  /**
   * Calibrate the gyro by running for a number of samples and computing the
   * center value. Then use the center value as the Accumulator center value for
   * subsequent measurements.
   *
   * It's important to make sure that the robot is not moving while the
   * centering calculations are in progress, this is typically done when the
   * robot is first turned on while it's sitting at rest before the competition
   * starts.
   */
  void Calibrate();

  /**
   * Return the heading of the robot as a Rotation2d.
   *
   * The angle is continuous, that is it will continue from 360 to 361 degrees.
   * This allows algorithms that wouldn't want to see a discontinuity in the
   * gyro output as it sweeps past from 360 to 0 on the second time around.
   *
   * The angle is expected to increase as the gyro turns counterclockwise when
   * looked at from the top. It needs to follow the NWU axis convention.
   *
   * @return the current heading of the robot as a Rotation2d. This heading is
   *         based on integration of the returned rate from the gyro.
   */
  Rotation2d GetRotation2d() const;

  /**
   * Get the SPI port number.
   *
   * @return The SPI port number.
   */
  int GetPort() const;

  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  SPI m_spi;
  SPI::Port m_port;
  bool m_connected{false};

  hal::SimDevice m_simDevice;
  hal::SimBoolean m_simConnected;
  hal::SimDouble m_simAngle;
  hal::SimDouble m_simRate;

  uint16_t ReadRegister(int reg);
};

}  // namespace frc
