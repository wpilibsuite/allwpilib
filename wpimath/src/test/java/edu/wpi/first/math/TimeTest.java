// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math;

import edu.wpi.first.math.estimator.DifferentialDrivePoseEstimator;
import edu.wpi.first.math.estimator.DifferentialDrivePoseEstimator3d;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Pose3d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.kinematics.DifferentialDriveKinematics;
import edu.wpi.first.math.kinematics.DifferentialDriveOdometry;
import edu.wpi.first.math.kinematics.DifferentialDriveOdometry3d;
import java.util.Arrays;
import org.junit.jupiter.api.Test;

class TimeTest {
  @SuppressWarnings("PMD.AvoidArrayLoops")
  private static void processDurations(long[] durations, String prefix) {
    long total_duration = 0;
    for (long duration : durations) {
      total_duration += duration;
    }

    double mean = (double) total_duration / durations.length;

    double sum_squares = 0;
    for (double duration : durations) {
      sum_squares += (duration - mean) * (duration - mean);
    }

    double std_dev = Math.sqrt(sum_squares / durations.length);

    System.out.println(prefix + "Mean: " + mean + ", Std dev: " + std_dev);

    long[] buffer = new long[10];

    for (int i = 0; i < 10; ++i) {
      buffer[i] = durations[i];
    }

    System.out.println(prefix + "First 10: " + Arrays.toString(buffer));

    for (int i = 0; i < 10; ++i) {
      buffer[i] = durations[durations.length - 10 + i];
    }

    System.out.println(prefix + "Last 10: " + Arrays.toString(buffer));

    long[] sorted = Arrays.copyOf(durations, durations.length);
    Arrays.sort(sorted);

    for (int i = 0; i < 10; ++i) {
      buffer[i] = sorted[i];
    }

    System.out.println(prefix + "Fastest 10: " + Arrays.toString(buffer));

    for (int i = 0; i < 10; ++i) {
      buffer[i] = sorted[sorted.length - 10 + i];
    }

    System.out.println(prefix + "Slowest 10: " + Arrays.toString(buffer));
  }

  private static void time(int count, Runnable action, Runnable setup, String prefix) {
    long[] durations = new long[count];

    for (int i = 0; i < count; ++i) {
      setup.run();
      long start = System.nanoTime();
      action.run();
      long end = System.nanoTime();
      durations[i] = end - start;
    }

    processDurations(durations, prefix);
  }

  void timeSuite(String name, Runnable action, Runnable setup) {
    System.out.println(name + ":");
    System.out.println("  Warmup: (100,000 iterations):");
    time(100_000, action, setup, "    ");
    for (int i = 0; i < 5; ++i) {
      System.out.println("  Run " + i + ":");
      time(1_000, action, setup, "    ");
    }
  }

  void timeSuite(String name, Runnable action) {
    timeSuite(name, action, () -> {});
  }

  @Test
  void testTime() {
    {
      var odometry = new DifferentialDriveOdometry(Rotation2d.kZero, 0, 0, Pose2d.kZero);
      var gyroAngle = Rotation2d.kZero;
      double leftDistance = 0;
      double rightDistance = 0;
      timeSuite(
          "Odometry update (2d)",
          () -> odometry.update(gyroAngle, leftDistance, rightDistance),
          () -> odometry.resetPosition(Rotation2d.kZero, 0, 0, Pose2d.kZero));
    }

    {
      var odometry = new DifferentialDriveOdometry3d(Rotation3d.kZero, 0, 0, Pose3d.kZero);
      var gyroAngle = Rotation3d.kZero;
      double leftDistance = 0;
      double rightDistance = 0;
      timeSuite(
          "Odometry update (3d)",
          () -> odometry.update(gyroAngle, leftDistance, rightDistance),
          () -> odometry.resetPosition(Rotation3d.kZero, 0, 0, Pose3d.kZero));
    }

    {
      var kinematics = new DifferentialDriveKinematics(1);
      var poseEstimator =
          new DifferentialDrivePoseEstimator(kinematics, Rotation2d.kZero, 0, 0, Pose2d.kZero);
      var gyroAngle = Rotation2d.kZero;
      double leftDistance = 0;
      double rightDistance = 0;
      timeSuite(
          "Pose estimator update (2d)",
          () -> poseEstimator.update(gyroAngle, leftDistance, rightDistance),
          () -> poseEstimator.resetPosition(Rotation2d.kZero, 0, 0, Pose2d.kZero));
    }

    {
      var kinematics = new DifferentialDriveKinematics(1);
      var poseEstimator =
          new DifferentialDrivePoseEstimator3d(kinematics, Rotation3d.kZero, 0, 0, Pose3d.kZero);
      var gyroAngle = Rotation3d.kZero;
      double leftDistance = 0;
      double rightDistance = 0;
      timeSuite(
          "Pose estimator update (3d)",
          () -> poseEstimator.update(gyroAngle, leftDistance, rightDistance),
          () -> poseEstimator.resetPosition(Rotation3d.kZero, 0, 0, Pose3d.kZero));
    }
  }
}
