// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Pose3d;
import edu.wpi.first.math.geometry.Quaternion;
import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.geometry.Twist3d;
import org.junit.jupiter.api.Test;

public class WPIMathJNITest {
  private static double[] toDoubleArray(Pose3d pose) {
    Quaternion quaternion = pose.getRotation().getQuaternion();
    return new double[] {
      pose.getX(),
      pose.getY(),
      pose.getZ(),
      quaternion.getW(),
      quaternion.getX(),
      quaternion.getY(),
      quaternion.getZ()
    };
  }

  private static double[] toDoubleArray(Twist3d twist) {
    return new double[] {twist.dx, twist.dy, twist.dz, twist.rx, twist.ry, twist.rz};
  }

  private static Pose3d pose3dFromDoubleArray(double[] arr) {
    return new Pose3d(
        arr[0], arr[1], arr[2], new Rotation3d(new Quaternion(arr[3], arr[4], arr[5], arr[6])));
  }

  private static Twist3d twist3dFromDoubleArray(double[] arr) {
    return new Twist3d(arr[0], arr[1], arr[2], arr[3], arr[4], arr[5]);
  }

  @Test
  public void testLink() {
    assertDoesNotThrow(WPIMathJNI::forceLoad);
  }

  @Test
  public void testExpPose3d() {
    final var start = new Pose3d(1, 0, 0, new Rotation3d(0, -Math.PI / 2, 0));
    final var twist = new Twist3d(0, 1, 0, Math.PI, 0, 0);
    final var expected = start.exp(twist);
    final var result =
        pose3dFromDoubleArray(WPIMathJNI.expPose3d(toDoubleArray(start), toDoubleArray(twist)));
    assertEquals(expected, result);
  }

  @Test
  public void testLogPose3d() {
    final var start = new Pose3d(1, 0, 0, new Rotation3d(0, -Math.PI / 2, 0));
    final var end = new Pose3d(0, 1, 0, new Rotation3d(Math.PI / 2, 0, 0));
    final var expected = start.log(end);
    final var result =
        twist3dFromDoubleArray(WPIMathJNI.logPose3d(toDoubleArray(start), toDoubleArray(end)));
    assertEquals(expected, result);
  }
}
