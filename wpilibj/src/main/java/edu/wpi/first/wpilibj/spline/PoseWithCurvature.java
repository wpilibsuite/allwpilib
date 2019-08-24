/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.spline;

import edu.wpi.first.wpilibj.geometry.Pose2d;

/**
 * Represents a pair of a pose and a curvature.
 */
@SuppressWarnings("MemberName")
public class PoseWithCurvature {
  // Represents the pose.
  public Pose2d poseMeters;

  // Represents the curvature.
  public double curvatureRadPerMeter;

  /**
   * Constructs a PoseWithCurvature.
   *
   * @param poseMeters           The pose.
   * @param curvatureRadPerMeter The curvature.
   */
  public PoseWithCurvature(Pose2d poseMeters, double curvatureRadPerMeter) {
    this.poseMeters = poseMeters;
    this.curvatureRadPerMeter = curvatureRadPerMeter;
  }

  /**
   * Constructs a PoseWithCurvature with default values.
   */
  public PoseWithCurvature() {
    poseMeters = new Pose2d();
  }
}
