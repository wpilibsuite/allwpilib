package org.wpilib.math.estimator;

import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.kinematics.*;
import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.linalg.VecBuilder;
import org.wpilib.math.numbers.N1;
import org.wpilib.math.numbers.N3;

public class DoubleFollowerWheelPoseEstimator extends PoseEstimator<DoubleFollowerWheelPositions> {
  public DoubleFollowerWheelPoseEstimator(
      DoubleFollowerWheelKinematics kinematics,
      DoubleFollowerWheelPositions wheelPositions,
      Pose2d initialPose) {
    this(
        kinematics,
        wheelPositions,
        initialPose,
        VecBuilder.fill(0.1, 0.1, 0.1),
        VecBuilder.fill(0.9, 0.9, 0.9));
  }

  public DoubleFollowerWheelPoseEstimator(
      DoubleFollowerWheelKinematics kinematics,
      DoubleFollowerWheelPositions wheelPositions,
      Pose2d initialPose,
      Matrix<N3, N1> stateStdDevs,
      Matrix<N3, N1> visionMeasurementStdDevs) {
    super(
        kinematics,
        new DoubleFollowerWheelOdometry(kinematics, wheelPositions, initialPose),
        stateStdDevs,
        visionMeasurementStdDevs);
  }

  public Pose2d update(DoubleFollowerWheelPositions wheelPositions) {
    return super.update(wheelPositions.gyroTheta, wheelPositions);
  }

  public Pose2d updateWithTime(double currentTime, DoubleFollowerWheelPositions wheelPositions) {
    return super.updateWithTime(currentTime, wheelPositions.gyroTheta, wheelPositions);
  }

  public void resetPosition(DoubleFollowerWheelPositions wheelPositions, Pose2d pose) {
    super.resetPosition(wheelPositions.gyroTheta, wheelPositions, pose);
  }
}
