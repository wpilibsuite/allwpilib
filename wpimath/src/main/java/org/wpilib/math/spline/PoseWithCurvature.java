// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.spline;

import edu.wpi.first.math.geometry.Pose2d;

/** Represents a pair of a pose and a curvature. */
public class PoseWithCurvature {
  /** Represents the pose. */
  public Pose2d pose;

  /** Represents the curvature in radians per meter. */
  public double curvature;

  /**
   * Constructs a PoseWithCurvature.
   *
   * @param pose The pose.
   * @param curvature The curvature in radians per meter.
   */
  public PoseWithCurvature(Pose2d pose, double curvature) {
    this.pose = pose;
    this.curvature = curvature;
  }

  /** Constructs a PoseWithCurvature with default values. */
  public PoseWithCurvature() {
    pose = Pose2d.kZero;
  }
}
