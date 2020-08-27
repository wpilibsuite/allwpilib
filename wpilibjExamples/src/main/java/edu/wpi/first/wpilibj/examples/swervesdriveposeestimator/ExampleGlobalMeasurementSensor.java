/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.swervesdriveposeestimator;

import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.math.StateSpaceUtil;
import edu.wpi.first.wpilibj.util.Units;
import edu.wpi.first.wpiutil.math.VecBuilder;

/**
 * This dummy class represents a global measurement sensor, such as a computer vision solution.
 */
public final class ExampleGlobalMeasurementSensor {
  private ExampleGlobalMeasurementSensor() {
    // Utility class
  }


  /**
   * Get a "noisy" fake global pose reading.
   *
   * @param estimatedRobotPose The robot pose.
   */
  public static Pose2d getEstimatedGlobalPose(Pose2d estimatedRobotPose) {
    var rand = StateSpaceUtil.makeWhiteNoiseVector(VecBuilder.fill(0.5, 0.5,
        Units.degreesToRadians(30)));
    return new Pose2d(estimatedRobotPose.getX() + rand.get(0, 0),
        estimatedRobotPose.getY() + rand.get(1, 0),
        estimatedRobotPose.getRotation().plus(new Rotation2d(rand.get(2, 0))));
  }

}
