// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;

import edu.wpi.first.math.geometry.Pose3d;
import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.geometry.Twist3d;
import org.junit.jupiter.api.Test;

public class WPIMathJNITest {
  @Test
  public void testLink() {
    assertDoesNotThrow(WPIMathJNI::forceLoad);
  }

  public static double timePose3dExpMicros(Pose3d start, Twist3d twist, int count) {
    final long startTime = System.nanoTime();
    for (int i = 0; i < count; i++) {
      start.exp(twist);
    }
    final long endTime = System.nanoTime();
    double totalTimeMicros = (endTime - startTime) / 1000.0;
    return totalTimeMicros / count;
  }

  public static double timePose3dLogMicros(Pose3d start, Pose3d end, int count) {
    final long startTime = System.nanoTime();
    for (int i = 0; i < count; i++) {
      start.log(end);
    }
    final long endTime = System.nanoTime();
    double totalTimeMicros = (endTime - startTime) / 1000.0;
    return totalTimeMicros / count;
  }

  @Test
  public void timePose3dExp() {
    final var start = new Pose3d(1, 0, 0, new Rotation3d(0, -Math.PI / 2, 0));
    final var twist = new Twist3d(0, 0, 0, Math.PI, 0, 0);
    final int warmupCount = 100_000;
    final int count = 1_000;
    var msg = new StringBuilder();
    msg.append("Pose3d.exp():\n");
    msg.append("  start = " + start + "\n");
    msg.append("  twist = " + twist + "\n");
    msg.append(
        "  warm up ("
            + warmupCount
            + " iterations): average "
            + timePose3dExpMicros(start, twist, warmupCount)
            + "µs\n");
    msg.append("  iterations per run: " + count + "\n");
    for (int i = 1; i <= 5; i++) {
      msg.append("  run " + i + ": average " + timePose3dExpMicros(start, twist, count) + "μs\n");
    }
    throw new RuntimeException("report:\n" + msg);
  }

  @Test
  public void timePose3dLog() {
    final var start = new Pose3d(1, 0, 0, new Rotation3d(0, -Math.PI / 2, 0));
    final var end = new Pose3d(0, 1, 0, new Rotation3d(Math.PI / 2, 0, 0));
    final int warmupCount = 100_000;
    final int count = 1_000;
    var msg = new StringBuilder();
    msg.append("Pose3d.log():\n");
    msg.append("  start = " + start + "\n");
    msg.append("  end = " + end + "\n");
    msg.append(
        "  warm up ("
            + warmupCount
            + " iterations): average "
            + timePose3dLogMicros(start, end, warmupCount)
            + "µs\n");
    msg.append("  iterations per run: " + count + "\n");
    for (int i = 1; i <= 5; i++) {
      msg.append("  run " + i + ": average " + timePose3dLogMicros(start, end, count) + "μs\n");
    }
    throw new RuntimeException("report:\n" + msg);
  }
}
