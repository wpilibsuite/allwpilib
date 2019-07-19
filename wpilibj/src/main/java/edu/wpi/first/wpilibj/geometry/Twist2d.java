/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.geometry;

/**
 * A change in distance along arc since the last pose update. We can use ideas
 * from differential calculus to create new Pose2ds from a Twist2d and vise
 * versa.
 *
 * <p>A Twist can be used to represent a difference between two poses.
 */
@SuppressWarnings("MemberName")
public class Twist2d {
  /**
   * Linear "dx" component.
   */
  public double dx;

  /**
   * Linear "dy" component.
   */
  public double dy;

  /**
   * Angular "dtheta" component (radians).
   */
  public double dtheta;

  public Twist2d() {
  }

  /**
   * Constructs a Twist2d with the given values.
   * @param dx Change in x direction relative to robot.
   * @param dy Change in y direction relative to robot.
   * @param dtheta Change in angle relative to robot.
   */
  public Twist2d(double dx, double dy, double dtheta) {
    this.dx = dx;
    this.dy = dy;
    this.dtheta = dtheta;
  }
}
