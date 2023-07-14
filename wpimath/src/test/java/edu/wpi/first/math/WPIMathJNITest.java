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

  public static double timePose3dExpMillis(Pose3d start, Twist3d twist, int count) {
    final long startTime = System.nanoTime();
    for (int i = 0; i < count; i++) {
      start.exp(twist);
    }
    final long endTime = System.nanoTime();
    double totalTimeMillis = (endTime - startTime) / 1000.0;
    return totalTimeMillis / count;
  }

  public static double timePose3dLogMillis(Pose3d start, Pose3d end, int count) {
    final long startTime = System.nanoTime();
    for (int i = 0; i < count; i++) {
      start.log(end);
    }
    final long endTime = System.nanoTime();
    double totalTimeMillis = (endTime - startTime) / 1000.0;
    return totalTimeMillis / count;
  }

  @Test
  public void timePose3dExp() {
    final var start = new Pose3d(1, 0, 0, new Rotation3d(0, -Math.PI / 2, 0));
    final var twist = new Twist3d(0, 0, 0, Math.PI, 0, 0);
    var msg = new StringBuilder();
    msg.append("Pose3d.exp():\n");
    msg.append("  start = " + start + "\n");
    msg.append("  twist = " + twist + "\n");
    msg.append("  average milliseconds: " + timePose3dExpMillis(start, twist, 1_000) + "\n");
    throw new RuntimeException("report:\n" + msg);
  }

  @Test
  public void timePose3dLog() {
    final var start = new Pose3d(1, 0, 0, new Rotation3d(0, -Math.PI / 2, 0));
    final var end = new Pose3d(0, 1, 0, new Rotation3d(Math.PI / 2, 0, 0));
    var msg = new StringBuilder();
    msg.append("Pose3d.log():\n");
    msg.append("  start = " + start + "\n");
    msg.append("  end = " + end + "\n");
    msg.append("  average milliseconds: " + timePose3dLogMillis(start, end, 1_000) + "\n");
    throw new RuntimeException("report:\n" + msg);
  }
}
