// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Pose3d;
import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.geometry.Twist3d;
import org.junit.jupiter.api.Test;

public class WPIMathJNITest {
  @Test
  public void testLink() {
    assertDoesNotThrow(WPIMathJNI::forceLoad);
  }

  @Test
  public void testExpPose3d() {
    final var start = new Pose3d(1, 0, 0, new Rotation3d(0, -Math.PI / 2, 0));
    final var twist = new Twist3d(0, 1, 0, Math.PI, 0, 0);
    final var expected = start.exp(twist);
    final var result = WPIMathJNI.expPose3d(start, twist);
    assertEquals(expected, result);
  }

  @Test
  public void testLogPose3d() {
    final var start = new Pose3d(1, 0, 0, new Rotation3d(0, -Math.PI / 2, 0));
    final var end = new Pose3d(0, 1, 0, new Rotation3d(Math.PI / 2, 0, 0));
    final var expected = start.log(end);
    final var result = WPIMathJNI.logPose3d(start, end);
    assertEquals(expected, result);
  }
}
