// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math;

import edu.wpi.first.math.geometry.Pose3d;
import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.geometry.Twist3d;
import edu.wpi.first.util.datalog.DataLog;
import edu.wpi.first.util.datalog.StringLogEntry;

public final class DevMain {
  private static double timePose3dExpMicros(
      StringLogEntry discardEntry, Pose3d start, Twist3d twist, int count) {
    long totalNanos = 0;
    for (int i = 0; i < count; i++) {
      final long startTime = System.nanoTime();
      final var result = start.exp(twist);
      final long endTime = System.nanoTime();
      totalNanos += endTime - startTime;
      discardEntry.append(result.toString());
    }
    double totalTimeMicros = totalNanos / 1000.0;
    return totalTimeMicros / count;
  }

  private static double timePose3dLogMicros(
      StringLogEntry discardEntry, Pose3d start, Pose3d end, int count) {
    long totalNanos = 0;
    for (int i = 0; i < count; i++) {
      final long startTime = System.nanoTime();
      final var result = start.log(end);
      final long endTime = System.nanoTime();
      totalNanos += endTime - startTime;
      discardEntry.append(result.toString());
    }
    double totalTimeMicros = totalNanos / 1000.0;
    return totalTimeMicros / count;
  }

  private static void timeSuitePose3dExp(StringLogEntry discardEntry) {
    final var start = new Pose3d(1, 0, 0, new Rotation3d(0, -Math.PI / 2, 0));
    final var twist = new Twist3d(0, 0, 0, Math.PI, 0, 0);
    final int warmupCount = 100_000;
    final int normalCount = 1_000;
    System.out.println("Pose3d.exp():");
    System.out.println("  start = " + start);
    System.out.println("  twist = " + twist);
    System.out.println(
        "  warm up ("
            + warmupCount
            + " iterations): average "
            + timePose3dExpMicros(discardEntry, start, twist, warmupCount)
            + "µs");
    System.out.println("  iterations per run: " + normalCount);
    for (int i = 1; i <= 5; i++) {
      System.out.println(
          "  run "
              + i
              + ": average "
              + timePose3dExpMicros(discardEntry, start, twist, normalCount)
              + "μs");
    }
  }

  private static void timeSuitePose3dLog(StringLogEntry discardEntry) {
    final var start = new Pose3d(1, 0, 0, new Rotation3d(0, -Math.PI / 2, 0));
    final var end = new Pose3d(0, 1, 0, new Rotation3d(Math.PI / 2, 0, 0));
    final int warmupCount = 100_000;
    final int normalCount = 1_000;
    System.out.println("Pose3d.log():");
    System.out.println("  start = " + start);
    System.out.println("  end = " + end);
    System.out.println(
        "  warm up ("
            + warmupCount
            + " iterations): average "
            + timePose3dLogMicros(discardEntry, start, end, warmupCount)
            + "µs");
    System.out.println("  iterations per run: " + normalCount);
    for (int i = 1; i <= 5; i++) {
      System.out.println(
          "  run "
              + i
              + ": average "
              + timePose3dLogMicros(discardEntry, start, end, normalCount)
              + "μs");
    }
  }

  /** Main entry point. */
  public static void main(String[] args) {
    final var datalog = new DataLog();
    final var discardEntry = new StringLogEntry(datalog, "discard");
    timeSuitePose3dExp(discardEntry);
    timeSuitePose3dLog(discardEntry);
  }

  private DevMain() {}
}
