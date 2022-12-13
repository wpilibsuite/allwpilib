// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.apriltag.jni;

import org.junit.jupiter.api.Test;

public class JNITest {
  @Test
  void jniLinkTest() {
    // Test to verify that the JNI test link works correctly.
    var detector = AprilTagJNI.aprilTagCreate("tag16h5", 2.0, 0.0, 1, false, false);
    AprilTagJNI.aprilTagDestroy(detector);
  }
}
