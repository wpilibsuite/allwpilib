// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.estimator;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Pose3d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.geometry.Transform2d;
import edu.wpi.first.math.geometry.Transform3d;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.geometry.Translation3d;
import edu.wpi.first.math.kinematics.ChassisSpeeds;
import edu.wpi.first.math.kinematics.DifferentialDriveKinematics;
import edu.wpi.first.math.trajectory.Trajectory;
import edu.wpi.first.math.trajectory.TrajectoryConfig;
import edu.wpi.first.math.trajectory.TrajectoryGenerator;
import java.util.List;
import java.util.Random;
import java.util.TreeMap;
import java.util.function.Function;
import org.junit.jupiter.api.Test;

class DifferentialDrivePoseEstimatorTest {
  @Test
  void testAccuracy() {
    var kinematics = new DifferentialDriveKinematics(1);

    var estimator =
        new DifferentialDrivePoseEstimator(
            kinematics,
            new Rotation2d(),
            0,
            0,
            new Pose2d(),
            VecBuilder.fill(0.02, 0.02, 0.01),
            VecBuilder.fill(0.1, 0.1, 0.1));
    var trajectory =
        TrajectoryGenerator.generateTrajectory(
            List.of(
                new Pose2d(0, 0, Rotation2d.fromDegrees(45)),
                new Pose2d(3, 0, Rotation2d.fromDegrees(-90)),
                new Pose2d(0, 0, Rotation2d.fromDegrees(135)),
                new Pose2d(-3, 0, Rotation2d.fromDegrees(-90)),
                new Pose2d(0, 0, Rotation2d.fromDegrees(45))),
            new TrajectoryConfig(2, 2));

    testFollowTrajectory(
        kinematics,
        estimator,
        trajectory,
        state ->
            new ChassisSpeeds(
                state.velocityMetersPerSecond,
                0,
                state.velocityMetersPerSecond * state.curvatureRadPerMeter),
        state -> state.poseMeters,
        trajectory.getInitialPose(),
        new Pose2d(0, 0, Rotation2d.fromDegrees(45)),
        0.02,
        0.1,
        0.25,
        true);
  }

  @Test
  void testBadInitialPose() {
    var kinematics = new DifferentialDriveKinematics(1);

    var estimator =
        new DifferentialDrivePoseEstimator(
            kinematics,
            new Rotation2d(),
            0,
            0,
            new Pose2d(),
            VecBuilder.fill(0.02, 0.02, 0.01),
            VecBuilder.fill(0.1, 0.1, 0.1));

    var trajectory =
        TrajectoryGenerator.generateTrajectory(
            List.of(
                new Pose2d(0, 0, Rotation2d.fromDegrees(45)),
                new Pose2d(3, 0, Rotation2d.fromDegrees(-90)),
                new Pose2d(0, 0, Rotation2d.fromDegrees(135)),
                new Pose2d(-3, 0, Rotation2d.fromDegrees(-90)),
                new Pose2d(0, 0, Rotation2d.fromDegrees(45))),
            new TrajectoryConfig(2, 2));

    for (int offset_direction_degs = 0; offset_direction_degs < 360; offset_direction_degs += 45) {
      for (int offset_heading_degs = 0; offset_heading_degs < 360; offset_heading_degs += 45) {
        var pose_offset = Rotation2d.fromDegrees(offset_direction_degs);
        var heading_offset = Rotation2d.fromDegrees(offset_heading_degs);

        var initial_pose =
            trajectory
                .getInitialPose()
                .plus(
                    new Transform2d(
                        new Translation2d(pose_offset.getCos(), pose_offset.getSin()),
                        heading_offset));

        testFollowTrajectory(
            kinematics,
            estimator,
            trajectory,
            state ->
                new ChassisSpeeds(
                    state.velocityMetersPerSecond,
                    0,
                    state.velocityMetersPerSecond * state.curvatureRadPerMeter),
            state -> state.poseMeters,
            initial_pose,
            new Pose2d(0, 0, Rotation2d.fromDegrees(45)),
            0.02,
            0.1,
            0.25,
            false);
      }
    }
  }

  void testFollowTrajectory(
      final DifferentialDriveKinematics kinematics,
      final DifferentialDrivePoseEstimator estimator,
      final Trajectory trajectory,
      final Function<Trajectory.State, ChassisSpeeds> chassisSpeedsGenerator,
      final Function<Trajectory.State, Pose2d> visionMeasurementGenerator,
      final Pose2d startingPose,
      final Pose2d endingPose,
      final double dt,
      final double visionUpdateRate,
      final double visionUpdateDelay,
      final boolean checkError) {
    double leftDistanceMeters = 0;
    double rightDistanceMeters = 0;

    estimator.resetPosition(
        new Rotation2d(), leftDistanceMeters, rightDistanceMeters, startingPose);

    var rand = new Random(3538);

    double t = 0.0;

    final TreeMap<Double, Pose2d> visionUpdateQueue = new TreeMap<>();

    double maxError = Double.NEGATIVE_INFINITY;
    double errorSum = 0;
    while (t <= trajectory.getTotalTimeSeconds()) {
      var groundTruthState = trajectory.sample(t);

      // We are due for a new vision measurement if it's been `visionUpdateRate` seconds since the
      // last vision measurement
      if (visionUpdateQueue.isEmpty() || visionUpdateQueue.lastKey() + visionUpdateRate < t) {
        Pose2d newVisionPose =
            visionMeasurementGenerator
                .apply(groundTruthState)
                .plus(
                    new Transform2d(
                        new Translation2d(rand.nextGaussian() * 0.1, rand.nextGaussian() * 0.1),
                        new Rotation2d(rand.nextGaussian() * 0.05)));

        visionUpdateQueue.put(t, newVisionPose);
      }

      // We should apply the oldest vision measurement if it has been `visionUpdateDelay` seconds
      // since it was measured
      if (!visionUpdateQueue.isEmpty() && visionUpdateQueue.firstKey() + visionUpdateDelay < t) {
        var visionEntry = visionUpdateQueue.pollFirstEntry();
        estimator.addVisionMeasurement(visionEntry.getValue(), visionEntry.getKey());
      }

      var chassisSpeeds = chassisSpeedsGenerator.apply(groundTruthState);

      var wheelSpeeds = kinematics.toWheelSpeeds(chassisSpeeds);

      leftDistanceMeters += wheelSpeeds.leftMetersPerSecond * dt;
      rightDistanceMeters += wheelSpeeds.rightMetersPerSecond * dt;

      var xHat =
          estimator.updateWithTime(
              t,
              groundTruthState
                  .poseMeters
                  .getRotation()
                  .plus(new Rotation2d(rand.nextGaussian() * 0.05))
                  .minus(trajectory.getInitialPose().getRotation()),
              leftDistanceMeters,
              rightDistanceMeters);

      double error =
          groundTruthState.poseMeters.getTranslation().getDistance(xHat.getTranslation());
      if (error > maxError) {
        maxError = error;
      }
      errorSum += error;

      t += dt;
    }

    assertEquals(
        endingPose.getX(), estimator.getEstimatedPosition().getX(), 0.08, "Incorrect Final X");
    assertEquals(
        endingPose.getY(), estimator.getEstimatedPosition().getY(), 0.08, "Incorrect Final Y");
    assertEquals(
        endingPose.getRotation().getRadians(),
        estimator.getEstimatedPosition().getRotation().getRadians(),
        0.15,
        "Incorrect Final Theta");

    if (checkError) {
      assertEquals(
          0.0, errorSum / (trajectory.getTotalTimeSeconds() / dt), 0.07, "Incorrect mean error");
      assertEquals(0.0, maxError, 0.2, "Incorrect max error");
    }
  }

  @Test
  void testSimultaneousVisionMeasurements() {
    // This tests for multiple vision measurements appled at the same time. The expected behavior
    // is that all measurements affect the estimated pose. The alternative result is that only one
    // vision measurement affects the outcome. If that were the case, after 1000 measurements, the
    // estimated pose would converge to that measurement.
    var kinematics = new DifferentialDriveKinematics(1);

    var estimator =
        new DifferentialDrivePoseEstimator(
            kinematics,
            new Rotation2d(),
            0,
            0,
            new Pose2d(1, 2, Rotation2d.fromDegrees(270)),
            VecBuilder.fill(0.02, 0.02, 0.01),
            VecBuilder.fill(0.1, 0.1, 0.1));

    estimator.updateWithTime(0, new Rotation2d(), 0, 0);

    var visionMeasurements =
        new Pose2d[] {
          new Pose2d(0, 0, Rotation2d.fromDegrees(0)),
          new Pose2d(3, 1, Rotation2d.fromDegrees(90)),
          new Pose2d(2, 4, Rotation2d.fromRadians(180)),
        };

    for (int i = 0; i < 1000; i++) {
      for (var measurement : visionMeasurements) {
        estimator.addVisionMeasurement(measurement, 0);
      }
    }

    for (var measurement : visionMeasurements) {
      var errorLog =
          "Estimator converged to one vision measurement: "
              + estimator.getEstimatedPosition().toString()
              + " -> "
              + measurement.toString();

      var dx = Math.abs(measurement.getX() - estimator.getEstimatedPosition().getX());
      var dy = Math.abs(measurement.getY() - estimator.getEstimatedPosition().getY());
      var dtheta =
          Math.abs(
              measurement.getRotation().getDegrees()
                  - estimator.getEstimatedPosition().getRotation().getDegrees());

      assertEquals(dx > 0.08 || dy > 0.08 || dtheta > 0.08, true, errorLog);
    }
  }

  @Test
  void testDiscardsStaleVisionMeasurements() {
    var kinematics = new DifferentialDriveKinematics(1);
    var estimator =
        new DifferentialDrivePoseEstimator(
            kinematics,
            new Rotation2d(),
            0,
            0,
            new Pose2d(),
            VecBuilder.fill(0.1, 0.1, 0.1),
            VecBuilder.fill(0.9, 0.9, 0.9));

    double time = 0;

    // Add enough measurements to fill up the buffer
    for (; time < 4; time += 0.02) {
      estimator.updateWithTime(time, new Rotation2d(), 0, 0);
    }

    var odometryPose = estimator.getEstimatedPosition();

    // Apply a vision measurement made 3 seconds ago
    // This test passes if this does not cause a ConcurrentModificationException.
    estimator.addVisionMeasurement(
        new Pose2d(new Translation2d(10, 10), new Rotation2d(0.1)),
        1,
        VecBuilder.fill(0.1, 0.1, 0.1));

    assertEquals(odometryPose.getX(), estimator.getEstimatedPosition().getX(), "Incorrect Final X");
    assertEquals(odometryPose.getY(), estimator.getEstimatedPosition().getY(), "Incorrect Final Y");
    assertEquals(
        odometryPose.getRotation().getRadians(),
        estimator.getEstimatedPosition().getRotation().getRadians(),
        "Incorrect Final Theta");
  }


  @Test
  void testAccuracy3d() {
    var kinematics = new DifferentialDriveKinematics(1);

    var estimator =
        new DifferentialDrivePoseEstimator(kinematics, new Rotation3d(), 0, 0, new Pose3d());
    var trajectory =
        TrajectoryGenerator.generateTrajectory(
            List.of(
                new Pose2d(0, 0, Rotation2d.fromDegrees(45)),
                new Pose2d(3, 0, Rotation2d.fromDegrees(-90)),
                new Pose2d(0, 0, Rotation2d.fromDegrees(135)),
                new Pose2d(-3, 0, Rotation2d.fromDegrees(-90)),
                new Pose2d(0, 0, Rotation2d.fromDegrees(45))),
            new TrajectoryConfig(2, 2));

    testFollowTrajectory3d(
        kinematics,
        estimator,
        trajectory,
        state ->
            new ChassisSpeeds(
                state.velocityMetersPerSecond,
                0,
                state.velocityMetersPerSecond * state.curvatureRadPerMeter),
        state -> state.poseMeters,
        new Pose3d(0, 0, 0, new Rotation3d(VecBuilder.fill(1, 1, 1))),
        new Pose3d(0, 0, 0, new Rotation3d(VecBuilder.fill(1, 1, 1))),
        0.02,
        0.1,
        0.25,
        true);
  }

  void testFollowTrajectory3d(
      final DifferentialDriveKinematics kinematics,
      final DifferentialDrivePoseEstimator estimator,
      final Trajectory trajectory,
      final Function<Trajectory.State, ChassisSpeeds> chassisSpeedsGenerator,
      final Function<Trajectory.State, Pose2d> visionMeasurementGenerator,
      final Pose3d startingPose,
      final Pose3d endingPose,
      final double dt,
      final double visionUpdateRate,
      final double visionUpdateDelay,
      final boolean checkError) {
    double leftDistanceMeters = 0;
    double rightDistanceMeters = 0;

    estimator.resetPosition(
        new Rotation3d(), leftDistanceMeters, rightDistanceMeters, startingPose);

    var rand = new Random(3538);

    double t = 0.0;

    System.out.print(
        "time, est_x, est_y, est_z, est_roll, est_pitch, est_yaw, true_x, true_y, true_z, true_roll, true_pitch, true_yaw\n");

    final TreeMap<Double, Pose3d> visionUpdateQueue = new TreeMap<>();
    final TreeMap<Double, Pose3d> visionUpdateRecord = new TreeMap<>();

    double maxError = Double.NEGATIVE_INFINITY;
    double errorSum = 0;
    while (t <= trajectory.getTotalTimeSeconds()) {
      var groundTruthState = trajectory.sample(t);

      // We are due for a new vision measurement if it's been `visionUpdateRate` seconds since the
      // last vision measurement
      if (visionUpdateQueue.isEmpty() || visionUpdateQueue.lastKey() + visionUpdateRate < t) {
        var gaussianNoise =
            new Transform3d(
                new Translation3d(
                    rand.nextGaussian() * 0.1,
                    rand.nextGaussian() * 0.1,
                    rand.nextGaussian() * 0.1),
                new Rotation3d(
                    VecBuilder.fill(
                        rand.nextGaussian() * 0.1,
                        rand.nextGaussian() * 0.1,
                        rand.nextGaussian() * 0.1)));
        var visionTransform2d =
            visionMeasurementGenerator.apply(groundTruthState).minus(trajectory.getInitialPose());
        var visionTransform3d =
            new Transform3d(
                new Translation3d(visionTransform2d.getX(), visionTransform2d.getY(), 0),
                new Rotation3d(0, 0, visionTransform2d.getRotation().getRadians()));

        Pose3d newVisionPose = startingPose.transformBy(visionTransform3d).plus(gaussianNoise);

        visionUpdateQueue.put(t, newVisionPose);
        visionUpdateRecord.put(t, newVisionPose);
      }

      // We should apply the oldest vision measurement if it has been `visionUpdateDelay` seconds
      // since it was measured
      if (!visionUpdateQueue.isEmpty() && visionUpdateQueue.firstKey() + visionUpdateDelay < t) {
        var visionEntry = visionUpdateQueue.pollFirstEntry();
        estimator.addVisionMeasurement(visionEntry.getValue(), visionEntry.getKey());
      }

      var chassisSpeeds = chassisSpeedsGenerator.apply(groundTruthState);

      var wheelSpeeds = kinematics.toWheelSpeeds(chassisSpeeds);

      leftDistanceMeters += wheelSpeeds.leftMetersPerSecond * dt;
      rightDistanceMeters += wheelSpeeds.rightMetersPerSecond * dt;

      var trajectoryRotation2d =
          groundTruthState
              .poseMeters
              .getRotation()
              .minus(trajectory.getInitialPose().getRotation());
      var trajectoryRotation3d = new Rotation3d(0, 0, trajectoryRotation2d.getRadians());

      var gaussianNoise =
          new Rotation3d(
              VecBuilder.fill(
                  0.01 * rand.nextGaussian(),
                  0.01 * rand.nextGaussian(),
                  0.01 * rand.nextGaussian()));

      var xHat =
          estimator.updateWithTime(
              t, trajectoryRotation3d.plus(gaussianNoise), leftDistanceMeters, rightDistanceMeters);

      var xHatRotationVector = xHat.getRotation().getQuaternion().toRotationVector();

      var poseFromStartingPosition = groundTruthState.poseMeters.minus(trajectory.getInitialPose());
      var poseFromStartingPosition3d =
          new Transform3d(
              new Translation3d(
                  poseFromStartingPosition.getX(), poseFromStartingPosition.getY(), 0),
              new Rotation3d(
                  VecBuilder.fill(0, 0, poseFromStartingPosition.getRotation().getRadians())));

      var groundTruthIn3dFrame = startingPose.plus(poseFromStartingPosition3d);
      var groundTruthIn3dFrameRotationVector =
          groundTruthIn3dFrame.getRotation().getQuaternion().toRotationVector();

      System.out.printf(
          "%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n",
          t,
          xHat.getX(),
          xHat.getY(),
          xHat.getZ(),
          xHatRotationVector.get(0, 0),
          xHatRotationVector.get(1, 0),
          xHatRotationVector.get(2, 0),
          groundTruthIn3dFrame.getX(),
          groundTruthIn3dFrame.getY(),
          groundTruthIn3dFrame.getZ(),
          groundTruthIn3dFrameRotationVector.get(0, 0),
          groundTruthIn3dFrameRotationVector.get(1, 0),
          groundTruthIn3dFrameRotationVector.get(2, 0));

      double error =
          startingPose
              .plus(poseFromStartingPosition3d)
              .getTranslation()
              .getDistance(xHat.getTranslation());
      if (error > maxError) {
        maxError = error;
      }
      errorSum += error;

      t += dt;
    }

    System.out.print("time, est_x, est_y, est_z, est_roll, est_pitch, est_yaw\n");

    while (!visionUpdateRecord.isEmpty()) {
      var visionUpdate = visionUpdateRecord.pollFirstEntry();
      var visionUpdateRotationVector =
          visionUpdate.getValue().getRotation().getQuaternion().toRotationVector();
      System.out.printf(
          "%f, %f, %f, %f, %f, %f, %f\n",
          visionUpdate.getKey(),
          visionUpdate.getValue().getX(),
          visionUpdate.getValue().getY(),
          visionUpdate.getValue().getZ(),
          visionUpdateRotationVector.get(0, 0),
          visionUpdateRotationVector.get(1, 0),
          visionUpdateRotationVector.get(2, 0));
    }

    assertEquals(
        endingPose.getX(), estimator.getEstimatedPosition3d().getX(), 0.08, "Incorrect Final X");
    assertEquals(
        endingPose.getY(), estimator.getEstimatedPosition3d().getY(), 0.08, "Incorrect Final Y");
    assertEquals(
        endingPose.getZ(), estimator.getEstimatedPosition3d().getZ(), 0.08, "Incorrect Final Z");

    var endingRotationVector = endingPose.getRotation().getQuaternion().toRotationVector();
    var estimatedRotationVector = endingPose.getRotation().getQuaternion().toRotationVector();

    assertEquals(
        endingRotationVector.get(0, 0),
        estimatedRotationVector.get(0, 0),
        0.15,
        "Incorrect Final RX");

    assertEquals(
        endingRotationVector.get(1, 0),
        estimatedRotationVector.get(1, 0),
        0.15,
        "Incorrect Final RY");

    assertEquals(
        endingRotationVector.get(2, 0),
        estimatedRotationVector.get(2, 0),
        0.15,
        "Incorrect Final RZ");

    if (checkError) {
      assertEquals(
          0.0, errorSum / (trajectory.getTotalTimeSeconds() / dt), 0.07, "Incorrect mean error");
      assertEquals(0.0, maxError, 0.2, "Incorrect max error");
    }
  }
}
