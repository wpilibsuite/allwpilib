/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include "GyroBase.h"
#include "simulation/SimGyro.h"

namespace frc {

class AnalogInput;
class AnalogModule;

/**
 * Use a rate gyro to return the robots heading relative to a starting position.
 *
 * The AnalogGyro class tracks the robots heading based on the starting
 * position. As the robot rotates the new heading is computed by integrating
 * the rate of rotation returned by the sensor. When the class is instantiated,
 * it does a short calibration routine where it samples the gyro while at rest
 * to determine the default offset. This is subtracted from each sample to
 * determine the heading. This gyro class must be used with a channel that is
 * assigned one of the Analog accumulators from the FPGA. See AnalogInput for
 * the current accumulator assignments.
 */
class AnalogGyro : public GyroBase {
 public:
  static const int kOversampleBits;
  static const int kAverageBits;
  static const double kSamplesPerSecond;
  static const double kCalibrationSampleTime;
  static const double kDefaultVoltsPerDegreePerSecond;

  explicit AnalogGyro(int channel);
  virtual ~AnalogGyro() = default;
  double GetAngle() const;
  void Calibrate() override;
  double GetRate() const;
  void Reset();

 private:
  void InitAnalogGyro(int channel);

  SimGyro* impl;

  std::shared_ptr<ITable> m_table;
};

}  // namespace frc
