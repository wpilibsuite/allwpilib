package org.wpilib.math.estimator;

import org.wpilib.math.geometry.Pose3d;
import org.wpilib.math.geometry.Rotation3d;
import org.wpilib.math.kinematics.*;
import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.linalg.VecBuilder;
import org.wpilib.math.numbers.N1;
import org.wpilib.math.numbers.N4;

public class TripleFollowerWheelPoseEstimator3d
    extends PoseEstimator3d<TripleFollowerWheelPositions> {
  public TripleFollowerWheelPoseEstimator3d(
      TripleFollowerWheelKinematics kinematics,
      Rotation3d gyroAngle,
      TripleFollowerWheelPositions wheelPositions,
      Pose3d initialPose) {
    this(
        kinematics,
        gyroAngle,
        wheelPositions,
        initialPose,
        VecBuilder.fill(0.02, 0.02, 0.02, 0.01),
        VecBuilder.fill(0.1, 0.1, 0.1, 0.1));
  }

  public TripleFollowerWheelPoseEstimator3d(
      TripleFollowerWheelKinematics kinematics,
      Rotation3d gyroAngle,
      TripleFollowerWheelPositions wheelPositions,
      Pose3d initialPose,
      Matrix<N4, N1> stateStdDevs,
      Matrix<N4, N1> visionMeasurementStdDevs) {
    super(
        kinematics,
        new TripleFollowerWheelOdometry3d(kinematics, gyroAngle, wheelPositions, initialPose),
        stateStdDevs,
        visionMeasurementStdDevs);
  }
}
