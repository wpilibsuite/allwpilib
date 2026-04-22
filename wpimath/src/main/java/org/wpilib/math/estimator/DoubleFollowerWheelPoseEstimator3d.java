package org.wpilib.math.estimator;

import org.wpilib.math.geometry.Pose3d;
import org.wpilib.math.geometry.Rotation3d;
import org.wpilib.math.kinematics.DoubleFollowerWheelKinematics;
import org.wpilib.math.kinematics.DoubleFollowerWheelOdometry3d;
import org.wpilib.math.kinematics.DoubleFollowerWheelPositions;
import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.linalg.VecBuilder;
import org.wpilib.math.numbers.N1;
import org.wpilib.math.numbers.N4;

public class DoubleFollowerWheelPoseEstimator3d
    extends PoseEstimator3d<DoubleFollowerWheelPositions> {
  public DoubleFollowerWheelPoseEstimator3d(
      DoubleFollowerWheelKinematics kinematics,
      Rotation3d gyroAngle,
      DoubleFollowerWheelPositions wheelPositions,
      Pose3d initialPose) {
    super(
        kinematics,
        new DoubleFollowerWheelOdometry3d(kinematics, gyroAngle, wheelPositions, initialPose),
        VecBuilder.fill(0.02, 0.02, 0.02, 0.01),
        VecBuilder.fill(0.1, 0.1, 0.1, 0.1));
  }

  public DoubleFollowerWheelPoseEstimator3d(
      DoubleFollowerWheelKinematics kinematics,
      Rotation3d gyroAngle,
      DoubleFollowerWheelPositions wheelPositions,
      Pose3d initialPose,
      Matrix<N4, N1> stateStdDevs,
      Matrix<N4, N1> visionMeasurementStdDevs) {
    super(
        kinematics,
        new DoubleFollowerWheelOdometry3d(kinematics, gyroAngle, wheelPositions, initialPose),
        stateStdDevs,
        visionMeasurementStdDevs);
  }
}
