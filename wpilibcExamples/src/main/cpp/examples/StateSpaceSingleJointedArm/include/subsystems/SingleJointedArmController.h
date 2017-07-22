/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <Eigen/Core>
#include <frc/controller/StateSpaceLoop.h>

#include "subsystems/SingleJointedArmCoeffs.h"

class SingleJointedArmController {
 public:
  // State tolerances in meters and meters/sec respectively.
  static constexpr double kAngleTolerance = 0.05;
  static constexpr double kAngularVelocityTolerance = 2.0;

  SingleJointedArmController();

  SingleJointedArmController(SingleJointedArmController&&) = default;
  SingleJointedArmController& operator=(SingleJointedArmController&&) = default;

  void Enable();
  void Disable();

  /**
   * Sets the references.
   *
   * @param angle           Angle of arm in radians.
   * @param angularVelocity Angular velocity of arm in radians per second.
   */
  void SetReferences(double angle, double angularVelocity);

  bool AtReferences() const;

  /**
   * Sets the current encoder measurement.
   *
   * @param measuredAngle Angle of arm in radians.
   */
  void SetMeasuredAngle(double measuredAngle);

  /**
   * Returns the control loop calculated voltage.
   */
  double ControllerVoltage() const;

  /**
   * Returns the estimated angle.
   */
  double EstimatedAngle() const;

  /**
   * Returns the estimated angular velocity.
   */
  double EstimatedAngularVelocity() const;

  /**
   * Returns the error between the angle reference and the angle estimate.
   */
  double AngleError() const;

  /**
   * Returns the error between the angular velocity reference and the angular
   * velocity estimate.
   */
  double AngularVelocityError() const;

  /**
   * Executes the control loop for a cycle.
   */
  void Update();

  /**
   * Resets any internal state.
   */
  void Reset();

 private:
  // The current sensor measurement.
  Eigen::Matrix<double, 1, 1> m_Y;

  // The control loop.
  frc::StateSpaceLoop<2, 1, 1> m_loop{MakeSingleJointedArmLoop()};

  bool m_atReferences = false;
};
