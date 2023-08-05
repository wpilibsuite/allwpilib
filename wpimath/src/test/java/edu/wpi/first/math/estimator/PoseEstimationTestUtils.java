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
    private Pose2d pose;

    public SE2KinematicPrimitive(final Pose2d pose) {
      this.pose = pose;
    }

    @Override
    public SE2KinematicPrimitive copy() {
      return this;
    }

    @Override
    public SE2KinematicPrimitive minus(SE2KinematicPrimitive other) {
      return new SE2KinematicPrimitive(new Pose2d().exp(other.pose.log(this.pose)));
    }

    @Override
    public SE2KinematicPrimitive interpolate(SE2KinematicPrimitive endValue, double t) {
      return new SE2KinematicPrimitive(this.pose.interpolate(endValue.pose, t));
    }
  }

  protected static class SE2Kinematics
      implements Kinematics<SE2KinematicPrimitive, SE2KinematicPrimitive> {
    private double timeConstant;

    public SE2Kinematics(double timeConstant) {
      this.timeConstant = timeConstant;
    }

    @Override
    public ChassisSpeeds toChassisSpeeds(SE2KinematicPrimitive wheelSpeeds) {
      var twist = toTwist2d(wheelSpeeds);

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
    public Twist2d toTwist2d(SE2KinematicPrimitive wheelDeltas) {
      return new Pose2d().log(wheelDeltas.pose);
    }
  }
}
