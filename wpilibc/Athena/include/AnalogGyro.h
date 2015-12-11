/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "GyroBase.h"

class AnalogInput;

/**
 * Use a rate gyro to return the robots heading relative to a starting position.
 * The Gyro class tracks the robots heading based on the starting position. As
 * the robot
 * rotates the new heading is computed by integrating the rate of rotation
 * returned
 * by the sensor. When the class is instantiated, it does a short calibration
 * routine
 * where it samples the gyro while at rest to determine the default offset. This
 * is
 * subtracted from each sample to determine the heading. This gyro class must be
 * used
 * with a channel that is assigned one of the Analog accumulators from the FPGA.
 * See
 * AnalogInput for the current accumulator assignments.
 *
 * This class is for gyro sensors that connect to an analog input.
 */
class AnalogGyro : public GyroBase {
 public:
  static const uint32_t kOversampleBits = 10;
  static const uint32_t kAverageBits = 0;
  static constexpr float kSamplesPerSecond = 50.0;
  static constexpr float kCalibrationSampleTime = 5.0;
  static constexpr float kDefaultVoltsPerDegreePerSecond = 0.007;

  explicit AnalogGyro(int32_t channel);
  explicit AnalogGyro(AnalogInput *channel);
  explicit AnalogGyro(std::shared_ptr<AnalogInput> channel);
  AnalogGyro(int32_t channel, uint32_t center, float offset);
  AnalogGyro(std::shared_ptr<AnalogInput> channel, uint32_t center, float offset);
  virtual ~AnalogGyro() = default;

  float GetAngle() const override;
  double GetRate() const override;
  virtual uint32_t GetCenter() const;
  virtual float GetOffset() const;
  void SetSensitivity(float voltsPerDegreePerSecond);
  void SetDeadband(float volts);
  void Reset() override;
  virtual void InitGyro();
  void Calibrate() override;

  std::string GetSmartDashboardType() const override;

 protected:
  std::shared_ptr<AnalogInput> m_analog;

 private:
  float m_voltsPerDegreePerSecond;
  float m_offset;
  uint32_t m_center;
};
