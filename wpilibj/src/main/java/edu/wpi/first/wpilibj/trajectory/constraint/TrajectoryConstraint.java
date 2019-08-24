/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.trajectory.constraint;

import edu.wpi.first.wpilibj.geometry.Pose2d;

public interface TrajectoryConstraint {

  double getMaxVelocityMetersPerSecond(Pose2d pose, double curvatureRadPerMeter);

  MinMax getMinMaxAccelerationMetersPerSecondSq(Pose2d pose, double curvatureRadPerMeter,
                                                double velocityMetersPerSecond);

  @SuppressWarnings("MemberName")
  class MinMax {
    public double minAccelerationMetersPerSecondSq = -Double.MAX_VALUE;
    public double maxAccelerationMetersPerSecondSq = +Double.MAX_VALUE;

    public MinMax(double minAccelerationMetersPerSecondSq,
                  double maxAccelerationMetersPerSecondSq) {
      this.minAccelerationMetersPerSecondSq = minAccelerationMetersPerSecondSq;
      this.maxAccelerationMetersPerSecondSq = maxAccelerationMetersPerSecondSq;
    }

    public MinMax() {
    }
  }
}
