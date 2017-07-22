/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <Eigen/Core>
#include <frc/controller/StateSpaceLoop.h>

#include "subsystems/DifferentialDriveCoeffs.h"

class DifferentialDriveController {
 public:
  // State tolerances in meters and meters/sec respectively.
  static constexpr double kPositionTolerance = 0.05;
  static constexpr double kVelocityTolerance = 2.0;

  DifferentialDriveController();

  DifferentialDriveController(DifferentialDriveController&&) = default;
  DifferentialDriveController& operator=(DifferentialDriveController&&) =
      default;

  void Enable();
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

  bool AtReferences() const;

  /**
   * Sets the current encoder measurements.
   *
   * @param leftPosition  Position of left side in meters.
   * @param rightPosition Position of right side in meters.
   */
  void SetMeasuredStates(double leftPosition, double rightPosition);

  /**
   * Returns the control loop calculated voltage for the left side.
   */
  double ControllerLeftVoltage() const;

  /**
   * Returns the control loop calculated voltage for the left side.
   */
  double ControllerRightVoltage() const;

  /**
   * Returns the estimated left position.
   */
  double EstimatedLeftPosition() const;

  /**
   * Returns the estimated left velocity.
   */
  double EstimatedLeftVelocity() const;

  /**
   * Returns the estimated right position.
   */
  double EstimatedRightPosition() const;

  /**
   * Returns the estimated right velocity.
   */
  double EstimatedRightVelocity() const;

  /**
   * Returns the error between the left position reference and the left position
   * estimate.
   */
  double LeftPositionError() const;

  /**
   * Returns the error between the left velocity reference and the left velocity
   * estimate.
   */
  double LeftVelocityError() const;

  /**
   * Returns the error between the right position reference and the right
   * position estimate.
   */
  double RightPositionError() const;

  /**
   * Returns the error between the right velocity reference and the right
   * velocity estimate.
   */
  double RightVelocityError() const;

  /**
   * Executes the control loop for a cycle.
   */
  void Update();

  /**
   * Resets any internal state.
   */
  void Reset();

 private:
  // The current sensor measurements.
  Eigen::Matrix<double, 2, 1> m_Y;

  // The control loop.
  frc::StateSpaceLoop<4, 2, 2> m_loop{MakeDifferentialDriveLoop()};

  bool m_atReferences = false;
};
