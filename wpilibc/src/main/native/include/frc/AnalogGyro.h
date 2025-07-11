// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <hal/Types.h>
#include <wpi/telemetry/TelemetryLoggable.h>

#include "frc/geometry/Rotation2d.h"

namespace frc {

class AnalogInput;

/**
 * Use a rate gyro to return the robots heading relative to a starting position.
 * The Gyro class tracks the robots heading based on the starting position. As
 * the robot rotates the new heading is computed by integrating the rate of
 * rotation returned by the sensor. When the class is instantiated, it does a
 * short calibration routine where it samples the gyro while at rest to
 * determine the default offset. This is subtracted from each sample to
 * determine the heading. This gyro class must be used with a channel that is
 * assigned one of the Analog accumulators from the FPGA. See AnalogInput for
 * the current accumulator assignments.
 *
 * This class is for gyro sensors that connect to an analog input.
 */
class AnalogGyro : public wpi::TelemetryLoggable {
 public:
  /**
   * %Gyro constructor using the Analog Input channel number.
   *
   * @param channel The analog channel the gyro is connected to. Gyros can only
   *                be used on on-board Analog Inputs 0-1.
   */
  explicit AnalogGyro(int channel) {}

  /**
   * Gyro constructor with a precreated AnalogInput object.
   *
   * Use this constructor when the analog channel needs to be shared.
   * This object will not clean up the AnalogInput object when using this
   * constructor.
   *
   * Gyros can only be used on on-board channels 0-1.
   *
   * @param channel A pointer to the AnalogInput object that the gyro is
   *                connected to.
   */
  explicit AnalogGyro(AnalogInput* channel) {}

  /**
   * %Gyro constructor with a precreated AnalogInput object.
   *
   * Use this constructor when the analog channel needs to be shared.
   * This object will not clean up the AnalogInput object when using this
   * constructor.
   *
   * @param channel A pointer to the AnalogInput object that the gyro is
   *                connected to.
   */
  explicit AnalogGyro(std::shared_ptr<AnalogInput> channel) {}

  /**
   * %Gyro constructor using the Analog Input channel number with parameters for
   * presetting the center and offset values. Bypasses calibration.
   *
   * @param channel The analog channel the gyro is connected to. Gyros can only
   *                be used on on-board Analog Inputs 0-1.
   * @param center  Preset uncalibrated value to use as the accumulator center
   *                value.
   * @param offset  Preset uncalibrated value to use as the gyro offset.
   */
  AnalogGyro(int channel, int center, double offset) {}

  /**
   * %Gyro constructor with a precreated AnalogInput object and calibrated
   * parameters.
   *
   * Use this constructor when the analog channel needs to be shared.
   * This object will not clean up the AnalogInput object when using this
   * constructor.
   *
   * @param channel A pointer to the AnalogInput object that the gyro is
   *                connected to.
   * @param center  Preset uncalibrated value to use as the accumulator center
   *                value.
   * @param offset  Preset uncalibrated value to use as the gyro offset.
   */
  AnalogGyro(std::shared_ptr<AnalogInput> channel, int center, double offset) {}

  AnalogGyro(AnalogGyro&& rhs) = default;
  AnalogGyro& operator=(AnalogGyro&& rhs) = default;

  ~AnalogGyro() override = default;

  /**
   * Return the actual angle in degrees that the robot is currently facing.
   *
   * The angle is based on the current accumulator value corrected by the
   * oversampling rate, the gyro type and the A/D calibration values. The angle
   * is continuous, that is it will continue from 360->361 degrees. This allows
   * algorithms that wouldn't want to see a discontinuity in the gyro output as
   * it sweeps from 360 to 0 on the second time around.
   *
   * @return The current heading of the robot in degrees. This heading is based
   *         on integration of the returned rate from the gyro.
   */
  double GetAngle() const { return 0; }

  /**
   * Return the rate of rotation of the gyro
   *
   * The rate is based on the most recent reading of the gyro analog value
   *
   * @return the current rate in degrees per second
   */
  double GetRate() const { return 0; }

  /**
   * Return the gyro center value. If run after calibration,
   * the center value can be used as a preset later.
   *
   * @return the current center value
   */
  virtual int GetCenter() const { return 0; }

  /**
   * Return the gyro offset value. If run after calibration,
   * the offset value can be used as a preset later.
   *
   * @return the current offset value
   */
  virtual double GetOffset() const { return 0; }

  /**
   * Set the gyro sensitivity.
   *
   * This takes the number of volts/degree/second sensitivity of the gyro and
   * uses it in subsequent calculations to allow the code to work with multiple
   * gyros. This value is typically found in the gyro datasheet.
   *
   * @param voltsPerDegreePerSecond The sensitivity in Volts/degree/second
   */
  void SetSensitivity(double voltsPerDegreePerSecond) {}

  /**
   * Set the size of the neutral zone.
   *
   * Any voltage from the gyro less than this amount from the center is
   * considered stationary.  Setting a deadband will decrease the amount of
   * drift when the gyro isn't rotating, but will make it less accurate.
   *
   * @param volts The size of the deadband in volts
   */
  void SetDeadband(double volts) {}

  /**
   * Reset the gyro.
   *
   * Resets the gyro to a heading of zero. This can be used if there is
   * significant drift in the gyro and it needs to be recalibrated after it has
   * been running.
   */
  void Reset() {}

  /**
   * Initialize the gyro.
   *
   * Calibration is handled by Calibrate().
   */
  void InitGyro() {}

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
  void Calibrate() {}

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
  Rotation2d GetRotation2d() const { return {}; }

  /**
   * Gets the analog input for the gyro.
   *
   * @return AnalogInput
   */
  std::shared_ptr<AnalogInput> GetAnalogInput() const { return nullptr; }

  void UpdateTelemetry(wpi::TelemetryTable& table) const override {}
};

}  // namespace frc
