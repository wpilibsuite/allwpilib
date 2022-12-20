// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.apriltag.jni;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;

public class JNITest {
  @Test
  void validCreationTest() {
    var detector = AprilTagJNI.aprilTagCreate("tag16h5", 2.0, 0.0, 1, false, false);
    assertTrue(detector >= 0);
    AprilTagJNI.aprilTagDestroy(detector);
  }

  @Test
  void invalidCreationTest() {
    var detector = AprilTagJNI.aprilTagCreate("badtag", 2.0, 0.0, 1, false, false);
    assertEquals(detector, -1);
    // Still call destroy to ensure passing a bad destroy value doesn't break
    AprilTagJNI.aprilTagDestroy(detector);
  }
}
