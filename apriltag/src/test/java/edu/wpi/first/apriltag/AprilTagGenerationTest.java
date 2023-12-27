// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.apriltag;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.util.PixelFormat;
import org.junit.jupiter.api.Test;

class AprilTagGenerationTest {
  @Test
  void test36h11() {
    var frame = AprilTag.generate36h11AprilTagImage(1);
    assertEquals(PixelFormat.kGray, frame.getPixelFormat());
    assertEquals(10, frame.getWidth());
    assertEquals(10, frame.getHeight());
    assertEquals(96, frame.getStride()); // default stride in apriltag library
    assertEquals(960, frame.getSize());
    // check the diagonal values
    var data = frame.getData();
    assertEquals(-1, data.get(96 * 0 + 0)); // outer border is white
    assertEquals(0, data.get(96 * 1 + 1)); // inner border is black
    assertEquals(-1, data.get(96 * 2 + 2));
    assertEquals(-1, data.get(96 * 3 + 3));
    assertEquals(-1, data.get(96 * 4 + 4));
    assertEquals(0, data.get(96 * 5 + 5));
    assertEquals(0, data.get(96 * 6 + 6));
    assertEquals(-1, data.get(96 * 7 + 7));
    assertEquals(0, data.get(96 * 8 + 8)); // inner border
    assertEquals(-1, data.get(96 * 9 + 9)); // outer border
  }
}
