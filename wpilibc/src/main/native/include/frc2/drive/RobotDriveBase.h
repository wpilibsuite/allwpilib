// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <wpi/ArrayRef.h>

namespace frc2 {

/**
 * Common base class for drive platforms.
 */
class RobotDriveBase {
 public:
  RobotDriveBase() = default;

  RobotDriveBase(RobotDriveBase&&) = default;
  RobotDriveBase& operator=(RobotDriveBase&&) = default;

  /**
   * Configure the scaling factor for using RobotDrive with motor controllers in
   * a mode other than PercentVbus or to limit the maximum output.
   *
   * @param maxOutput Multiplied with the output percentage computed by the
   *                  drive functions.
   */
  void SetMaxOutput(double maxOutput);

 protected:
  /**
   * The location of a motor on the robot for the purpose of driving.
   */
  enum MotorType {
    kFrontLeft = 0,
    kFrontRight = 1,
    kRearLeft = 2,
    kRearRight = 3,
    kLeft = 0,
    kRight = 1,
    kBack = 2
  };

  /**
   * Normalize all wheel speeds if the magnitude of any wheel is greater than
   * 1.0.
   *
   * @param wheelSpeeds Array of wheel speeds.
   */
  static void Normalize(wpi::MutableArrayRef<double> wheelSpeeds);

  double m_maxOutput = 1.0;
};

}  // namespace frc2
