// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.path.TravelingSalesman;
import java.util.concurrent.TimeUnit;
import org.openjdk.jmh.annotations.Benchmark;
import org.openjdk.jmh.annotations.BenchmarkMode;
import org.openjdk.jmh.annotations.Mode;
import org.openjdk.jmh.annotations.OutputTimeUnit;
import org.openjdk.jmh.profile.GCProfiler;
import org.openjdk.jmh.runner.Runner;
import org.openjdk.jmh.runner.RunnerException;
import org.openjdk.jmh.runner.options.Options;
import org.openjdk.jmh.runner.options.OptionsBuilder;
import org.openjdk.jmh.runner.options.TimeValue;

public class Main {
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

  /**
   * Main function.
   *
   * @param args The (unused) arguments to the program.
   */
  public static void main(String... args) throws RunnerException {
    Options opt =
        new OptionsBuilder()
            .include(Main.class.getSimpleName())
            .addProfiler(GCProfiler.class)
            .forks(1)
            .warmupIterations(2)
            .warmupTime(TimeValue.seconds(3))
            .measurementIterations(3)
            .measurementTime(TimeValue.seconds(3))
            .build();

    new Runner(opt).run();
  }

  @Benchmark
  @BenchmarkMode(Mode.AverageTime)
  @OutputTimeUnit(TimeUnit.MICROSECONDS)
  public Pose2d[] transform() {
    return transformTraveler.solve(poses, iterations);
  }

  @Benchmark
  @BenchmarkMode(Mode.AverageTime)
  @OutputTimeUnit(TimeUnit.MICROSECONDS)
  public Pose2d[] twist() {
    return twistTraveler.solve(poses, iterations);
  }
}
