// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import org.wpilib.math.geometry.Pose3d;
import org.wpilib.math.geometry.Rotation3d;
import org.wpilib.math.util.MathSharedStore;

public class TripleFollowerWheelOdometry3d extends Odometry3d<TripleFollowerWheelPositions> {
  public TripleFollowerWheelOdometry3d(
      TripleFollowerWheelKinematics kinematics,
      Rotation3d gyroAngle,
      TripleFollowerWheelPositions wheelPositions,
      Pose3d initialPose) {
    super(kinematics, gyroAngle, wheelPositions, initialPose);

    MathSharedStore.reportUsage("TripleFollowerWheelOdometry3d", "");
  }
}
