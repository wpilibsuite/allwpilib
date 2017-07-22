/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <Eigen/Core>
#include <frc/Encoder.h>
#include <frc/Notifier.h>
#include <frc/Spark.h>
#include <frc/SpeedControllerGroup.h>
#include <frc/drive/DifferentialDrive.h>

#include "subsystems/DifferentialDriveController.h"

class Drivetrain {
 public:
  Drivetrain();

  Drivetrain(const Drivetrain&) = delete;
  Drivetrain& operator=(const Drivetrain&) = delete;

  /**
   * Enable controller.
   */
  void Enable();

  /**
   * Disable controller.
   */
  void Disable();

  /**
   * Sets the references.
   *
   * @param leftPosition  Position of left side in meters.
   * @param leftVelocity  Velocity of left side in meters per second.
   * @param rightPosition Position of right side in meters.
   * @param rightVelocity Velocity of right side in meters per second.
   */
  void SetReferences(double leftPosition, double leftVelocity,
                     double rightPosition, double rightVelocity);

  bool AtReference() const;

  /**
   * Iterates the drivetrain control loop one cycle.
   */
  void Iterate();

  /**
   * Returns controller output for left side.
   */
  double ControllerLeftVoltage() const;

  /**
   * Returns controller output for right side.
   */
  double ControllerRightVoltage() const;

  void Reset();

 private:
  frc::Spark m_leftFront{1};
  frc::Spark m_leftRear{2};
  frc::SpeedControllerGroup m_left{m_leftFront, m_leftRear};
  frc::Encoder m_leftEncoder{1, 2};

  frc::Spark m_rightFront{3};
  frc::Spark m_rightRear{4};
  frc::SpeedControllerGroup m_right{m_rightFront, m_rightRear};
  frc::Encoder m_rightEncoder{1, 2};

  frc::DifferentialDrive m_drive{m_left, m_right};

  DifferentialDriveController m_drivetrain;
  frc::Notifier m_thread{&Drivetrain::Iterate, this};
  std::chrono::steady_clock::time_point m_lastTime =
      std::chrono::steady_clock::time_point::min();
};
