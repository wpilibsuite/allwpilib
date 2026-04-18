package org.wpilib.math.estimator;

import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.kinematics.TripleFollowerWheelKinematics;
import org.wpilib.math.kinematics.TripleFollowerWheelOdometry;
import org.wpilib.math.kinematics.TripleFollowerWheelPositions;
import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.linalg.VecBuilder;
import org.wpilib.math.numbers.N1;
import org.wpilib.math.numbers.N3;

public class TripleFollowerWheelPoseEstimator extends PoseEstimator<TripleFollowerWheelPositions> {
  public TripleFollowerWheelPoseEstimator(
      TripleFollowerWheelKinematics kinematics,
      Rotation2d gyroAngle,
      TripleFollowerWheelPositions wheelPositions,
      Pose2d initialPose) {
    this(
        kinematics,
        gyroAngle,
        wheelPositions,
        initialPose,
        VecBuilder.fill(0.1, 0.1, 0.1),
        VecBuilder.fill(0.9, 0.9, 0.9));
  }

  public TripleFollowerWheelPoseEstimator(
      TripleFollowerWheelKinematics kinematics,
      Rotation2d gyroAngle,
      TripleFollowerWheelPositions wheelPositions,
      Pose2d initialPose,
      Matrix<N3, N1> stateStdDevs,
      Matrix<N3, N1> visionMeasurementStdDevs) {
    super(
        kinematics,
        new TripleFollowerWheelOdometry(kinematics, gyroAngle, wheelPositions, initialPose),
        stateStdDevs,
        visionMeasurementStdDevs);
  }
}
