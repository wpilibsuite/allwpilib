// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package wpilib.robot;

import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.path.TravelingSalesman;

public final class TravelingSalesmanBenchmark {
  private TravelingSalesmanBenchmark() {
    // Utility class.
  }

  private static final Pose2d[] poses = {
    new Pose2d(-1, 1, Rotation2d.kCW_90deg),
    new Pose2d(-1, 2, Rotation2d.kCCW_90deg),
    new Pose2d(0, 0, Rotation2d.kZero),
    new Pose2d(0, 3, Rotation2d.kCW_90deg),
    new Pose2d(1, 1, Rotation2d.kCCW_90deg),
    new Pose2d(1, 2, Rotation2d.kCCW_90deg),
  };
  private static final int iterations = 100;

  private static final TravelingSalesman transformTraveler =
      new TravelingSalesman(
          (pose1, pose2) -> {
            var transform = pose2.minus(pose1);
            return Math.hypot(transform.getX(), transform.getY());
          });
  private static final TravelingSalesman twistTraveler =
      new TravelingSalesman(
          (pose1, pose2) -> {
            var twist = pose2.minus(pose1).log();
            return Math.hypot(twist.dx, twist.dy);
          });

  /** TravelingSalesman transform benchmark. */
  public static Pose2d[] transform() {
    return transformTraveler.solve(poses, iterations);
  }

  /** TravelingSalesman twist benchmark. */
  public static Pose2d[] twist() {
    return twistTraveler.solve(poses, iterations);
  }
}
