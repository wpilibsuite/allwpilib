/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

/**
 * A movement along an arc at constant curvature and velocity. We can use ideas
 * from differential calculus to create new Pose2ds from a Twist2d and vise
 * versa.
 *
 * A Twist can be used to represent a difference between two poses, a velocity,
 * an acceleration, etc.
 */
struct Twist2d {
  /**
   * Linear "dx" component
   */
  double dx;

  /**
   * Linear "dy" component
   */
  double dy;

  /**
   * Angular "dtheta" component (radians)
   */
  double dtheta;
};
