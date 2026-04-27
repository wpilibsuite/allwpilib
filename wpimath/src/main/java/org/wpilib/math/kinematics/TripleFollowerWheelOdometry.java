// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.util.MathSharedStore;

public class TripleFollowerWheelOdometry extends Odometry<TripleFollowerWheelPositions> {
  public TripleFollowerWheelOdometry(
      TripleFollowerWheelKinematics kinematics,
      Rotation2d gyroAngle,
      TripleFollowerWheelPositions wheelPositions,
      Pose2d initialPose) {
    super(kinematics, gyroAngle, wheelPositions, initialPose);

    MathSharedStore.reportUsage("TripleFollowerWheelOdometry", "");
  }

  public Pose2d update(Rotation2d gyroAngle, double x1, double x2, double y) {
    return super.update(gyroAngle, new TripleFollowerWheelPositions(x1, x2, y));
  }
}
