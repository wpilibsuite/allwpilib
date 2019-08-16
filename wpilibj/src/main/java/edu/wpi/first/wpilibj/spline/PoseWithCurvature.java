/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.spline;

import edu.wpi.first.wpilibj.geometry.Pose2d;

@SuppressWarnings("MemberName")
public class PoseWithCurvature {
  public Pose2d pose;
  public double curvature;

  public PoseWithCurvature(Pose2d pose, double curvature) {
    this.pose = pose;
    this.curvature = curvature;
  }

  public PoseWithCurvature() {
    pose = new Pose2d();
  }
}
