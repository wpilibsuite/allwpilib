// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.estimator;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Twist2d;
import edu.wpi.first.math.kinematics.ChassisSpeeds;
import edu.wpi.first.math.kinematics.Kinematics;
import edu.wpi.first.math.kinematics.WheelPositions;

public class PoseEstimationTestUtils {
  /**
   * This class is intended for use as "drivetrain-agnostic" kinematics. Odometry measurements are
   * akin to Poses, while their difference is akin to Twists.
   */
  protected static class SE2KinematicPrimitive implements WheelPositions<SE2KinematicPrimitive> {
    private final Pose2d pose;

    public SE2KinematicPrimitive(final Pose2d pose) {
      this.pose = pose;
    }

    @Override
    public SE2KinematicPrimitive copy() {
      return this;
    }

    @Override
    public SE2KinematicPrimitive interpolate(SE2KinematicPrimitive end, double t) {
      var twist = this.pose.log(end.pose);
      var scaled = new Twist2d(twist.dx * t, twist.dy * t, twist.dtheta * t);

      return new SE2KinematicPrimitive(this.pose.exp(scaled));
    }
  }

  protected static class SE2Kinematics
      implements Kinematics<SE2KinematicPrimitive, SE2KinematicPrimitive> {
    private final double timeConstant;

    public SE2Kinematics(double timeConstant) {
      this.timeConstant = timeConstant;
    }

    @Override
    public ChassisSpeeds toChassisSpeeds(SE2KinematicPrimitive wheelSpeeds) {
      var twist = new Pose2d().log(wheelSpeeds.pose);

      return new ChassisSpeeds(
          twist.dx * timeConstant, twist.dy * timeConstant, twist.dtheta * timeConstant);
    }

    @Override
    public SE2KinematicPrimitive toWheelSpeeds(ChassisSpeeds chassisSpeeds) {
      return new SE2KinematicPrimitive(
          new Pose2d()
              .exp(
                  new Twist2d(
                      chassisSpeeds.vxMetersPerSecond / timeConstant,
                      chassisSpeeds.vyMetersPerSecond / timeConstant,
                      chassisSpeeds.omegaRadiansPerSecond / timeConstant)));
    }

    @Override
    public Twist2d toTwist2d(SE2KinematicPrimitive start, SE2KinematicPrimitive end) {
      return start.pose.log(end.pose);
    }
  }
}
