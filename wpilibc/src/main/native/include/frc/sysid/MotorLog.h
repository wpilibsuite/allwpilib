// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

#include <units/angle.h>
#include <units/angular_velocity.h>
#include <units/length.h>
#include <units/velocity.h>
#include <units/voltage.h>

namespace frc::sysid {
/** Handle for recording SysId motor data to WPILog. */
class MotorLog {
 public:
  MotorLog() = default;
  virtual ~MotorLog() {}
  /**
   * Records a frame of motor data during a SysId routine. Data fields are
   * recorded as their own log entries (named like
   * "voltage-motorName-mechanismName"), and have their units specified in
   * metadata. Assumes a linear distance dimension, and records all data to
   * WPILog in meters.
   *
   * @param voltage Voltage applied to the motor.
   * @param distance Total linear displacement of the motor.
   * @param velocity Rate of change of linear displacement of the motor.
   * @param motorName Name of the motor, used as the topic name for the WPILog
   * entry for this data frame.
   */
  virtual void RecordFrameLinear(units::volt_t voltage, units::meter_t distance,
                                 units::meters_per_second_t velocity,
                                 const std::string& motorName) = 0;
  /**
   * Records a frame of motor data during a SysId routine. Data fields are
   * recorded as their own log entries (named like
   * "voltage-motorName-mechanismName"), and have their units specified in
   * metadata. Assumes a linear distance dimension, and records all data to
   * WPILog in rotations.
   *
   * @param voltage Voltage applied to the motor.
   * @param angle Total angular displacement of the motor.
   * @param velocity Rate of change of angular displacement of the motor.
   * @param motorName Name of the motor, used as the topic name for the WPILog
   * entry for this data frame.
   */
  virtual void RecordFrameAngular(units::volt_t voltage, units::turn_t angle,
                                  units::turns_per_second_t velocity,
                                  const std::string& motorName) = 0;

 private:
};
}  // namespace frc::sysid
