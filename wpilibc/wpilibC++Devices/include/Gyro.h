/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "SensorBase.h"
#include "PIDSource.h"
#include "LiveWindow/LiveWindowSendable.h"

#include <memory>

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
 */
class Gyro : public SensorBase, public PIDSource, public LiveWindowSendable {
 public:
  static const uint32_t kOversampleBits = 10;
  static const uint32_t kAverageBits = 0;
  static constexpr float kSamplesPerSecond = 50.0;
  static constexpr float kCalibrationSampleTime = 5.0;
  static constexpr float kDefaultVoltsPerDegreePerSecond = 0.007;

  explicit Gyro(int32_t channel);
  [[deprecated(
      "Raw pointers are deprecated; consider calling the Gyro constructor with "
      "a channel number or passing a shared_ptr instead.")]]
  explicit Gyro(AnalogInput *channel);
  explicit Gyro(std::shared_ptr<AnalogInput> channel);
  virtual ~Gyro() = default;
  virtual float GetAngle() const;
  virtual double GetRate() const;
  void SetSensitivity(float voltsPerDegreePerSecond);
  void SetDeadband(float volts);
  void SetPIDSourceParameter(PIDSourceParameter pidSource);
  virtual void Reset();
  void InitGyro();

  // PIDSource interface
  double PIDGet() const override;

  void UpdateTable() override;
  void StartLiveWindowMode() override;
  void StopLiveWindowMode() override;
  std::string GetSmartDashboardType() const override;
  void InitTable(std::shared_ptr<ITable> subTable) override;
  std::shared_ptr<ITable> GetTable() const override;

 protected:
  std::shared_ptr<AnalogInput> m_analog;

 private:
  float m_voltsPerDegreePerSecond;
  float m_offset;
  uint32_t m_center;
  PIDSourceParameter m_pidSource;

  std::shared_ptr<ITable> m_table = nullptr;
};
