// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.vision.apriltag;

import org.wpilib.math.geometry.Transform3d;

/** A pair of AprilTag pose estimates. */
@SuppressWarnings("MemberName")
public class AprilTagPoseEstimate {
  /**
   * Constructs a pose estimate.
   *
   * @param pose1 first pose
   * @param pose2 second pose
   * @param error1 error of first pose
   * @param error2 error of second pose
   */
  public AprilTagPoseEstimate(Transform3d pose1, Transform3d pose2, double error1, double error2) {
    this.pose1 = pose1;
    this.pose2 = pose2;
    this.error1 = error1;
    this.error2 = error2;
  }

  /**
   * Get the ratio of pose reprojection errors, called ambiguity. Numbers above 0.2 are likely to be
   * ambiguous.
   *
   * @return The ratio of pose reprojection errors.
   */
  public double getAmbiguity() {
    double min = Math.min(error1, error2);
    double max = Math.max(error1, error2);

    if (max > 0) {
      return min / max;
    } else {
      return -1;
    }
  }

  /** Pose 1. */
  public final Transform3d pose1;

  /** Pose 2. */
  public final Transform3d pose2;

  /** Object-space error of pose 1. */
  public final double error1;

  /** Object-space error of pose 2. */
  public final double error2;
}
