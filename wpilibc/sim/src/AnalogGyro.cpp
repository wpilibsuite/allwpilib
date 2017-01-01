/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "AnalogGyro.h"

#include <sstream>

#include "LiveWindow/LiveWindow.h"
#include "Timer.h"
#include "WPIErrors.h"

using namespace frc;

const int AnalogGyro::kOversampleBits = 10;
const int AnalogGyro::kAverageBits = 0;
const double AnalogGyro::kSamplesPerSecond = 50.0;
const double AnalogGyro::kCalibrationSampleTime = 5.0;
const double AnalogGyro::kDefaultVoltsPerDegreePerSecond = 0.007;

/**
 * Initialize the gyro.
 *
 * Calibrate the gyro by running for a number of samples and computing the
 * center value for this part. Then use the center value as the Accumulator
 * center value for subsequent measurements. It's important to make sure that
 * the robot is not moving while the centering calculations are in progress,
 * this is typically done when the robot is first turned on while it's sitting
 * at rest before the competition starts.
 */
void AnalogGyro::InitAnalogGyro(int channel) {
  SetPIDSourceType(PIDSourceType::kDisplacement);

  std::stringstream ss;
  ss << "analog/" << channel;
  impl = new SimGyro(ss.str());

  LiveWindow::GetInstance()->AddSensor("AnalogGyro", channel, this);
}

/**
 * AnalogGyro constructor with only a channel.
 *
 * @param channel The analog channel the gyro is connected to.
 */
AnalogGyro::AnalogGyro(int channel) { InitAnalogGyro(channel); }

/**
 * Reset the gyro.
 *
 * Resets the gyro to a heading of zero. This can be used if there is
 * significant drift in the gyro and it needs to be recalibrated after it has
 * been running.
 */
void AnalogGyro::Reset() { impl->Reset(); }

void AnalogGyro::Calibrate() { Reset(); }

/**
 * Return the actual angle in degrees that the robot is currently facing.
 *
 * The angle is based on the current accumulator value corrected by the
 * oversampling rate, the gyro type and the A/D calibration values. The angle
 * is continuous, that is can go beyond 360 degrees. This make algorithms that
 * wouldn't want to see a discontinuity in the gyro output as it sweeps past 0
 * on the second time around.
 *
 * @return the current heading of the robot in degrees. This heading is based on
 *         integration of the returned rate from the gyro.
 */
double AnalogGyro::GetAngle() const { return impl->GetAngle(); }

/**
 * Return the rate of rotation of the gyro
 *
 * The rate is based on the most recent reading of the gyro analog value
 *
 * @return the current rate in degrees per second
 */
double AnalogGyro::GetRate() const { return impl->GetVelocity(); }
