// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package wpilib.robot;

import java.util.concurrent.TimeUnit;
import org.openjdk.jmh.annotations.Benchmark;
import org.openjdk.jmh.annotations.BenchmarkMode;
import org.openjdk.jmh.annotations.Mode;
import org.openjdk.jmh.annotations.OutputTimeUnit;
import org.openjdk.jmh.profile.GCProfiler;
import org.openjdk.jmh.runner.Runner;
import org.openjdk.jmh.runner.RunnerException;
import org.openjdk.jmh.runner.options.OptionsBuilder;
import org.openjdk.jmh.runner.options.TimeValue;
import org.wpilib.math.geometry.Pose2d;

public class Main {
  /**
   * Main function.
   *
   * @param args The (unused) arguments to the program.
   */
  public static void main(String... args) throws RunnerException {
    var opt =
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
  public void cartPole() {
    CartPoleBenchmark.cartPole();
  }

  @Benchmark
  @BenchmarkMode(Mode.AverageTime)
  @OutputTimeUnit(TimeUnit.MICROSECONDS)
  public Pose2d[] travelingSalesmanTransform() {
    return TravelingSalesmanBenchmark.transform();
  }

  @Benchmark
  @BenchmarkMode(Mode.AverageTime)
  @OutputTimeUnit(TimeUnit.MICROSECONDS)
  public Pose2d[] travelingSalesmanTwist() {
    return TravelingSalesmanBenchmark.twist();
  }
}
