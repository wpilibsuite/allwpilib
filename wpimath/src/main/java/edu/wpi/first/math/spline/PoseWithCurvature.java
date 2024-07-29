// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.spline;

import edu.wpi.first.math.geometry.Pose2d;

/** Represents a pair of a pose and a curvature. */
public class PoseWithCurvature {
  /** Represents the pose. */
  public Pose2d poseMeters;

  /** Represents the curvature. */
  public double curvatureRadPerMeter;

  /**
   * Constructs a PoseWithCurvature.
   *
   * @param poseMeters The pose.
   * @param curvatureRadPerMeter The curvature.
   */
  public PoseWithCurvature(Pose2d poseMeters, double curvatureRadPerMeter) {
    this.poseMeters = poseMeters;
    this.curvatureRadPerMeter = curvatureRadPerMeter;
  }

  /** Constructs a PoseWithCurvature with default values. */
  public PoseWithCurvature() {
    poseMeters = Pose2d.kZero;
  }
}
