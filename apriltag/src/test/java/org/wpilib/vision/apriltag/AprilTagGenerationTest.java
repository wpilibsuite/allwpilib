// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.vision.apriltag;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.util.PixelFormat;

class AprilTagGenerationTest {
  @Test
  void test36h11() {
    var frame = AprilTag.generate36h11AprilTagImage(1);
    assertEquals(PixelFormat.kGray, frame.getPixelFormat());
    assertEquals(10, frame.getWidth());
    assertEquals(10, frame.getHeight());
    int stride = frame.getStride();
    assertEquals(stride * 10, frame.getSize());
    // check the diagonal values
    var data = frame.getData();
    assertEquals(-1, data.get(stride * 0 + 0)); // outer border is white
    assertEquals(0, data.get(stride * 1 + 1)); // inner border is black
    assertEquals(-1, data.get(stride * 2 + 2));
    assertEquals(-1, data.get(stride * 3 + 3));
    assertEquals(-1, data.get(stride * 4 + 4));
    assertEquals(0, data.get(stride * 5 + 5));
    assertEquals(0, data.get(stride * 6 + 6));
    assertEquals(-1, data.get(stride * 7 + 7));
    assertEquals(0, data.get(stride * 8 + 8)); // inner border
    assertEquals(-1, data.get(stride * 9 + 9)); // outer border
  }
}
