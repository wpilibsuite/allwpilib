// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.cscore;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotEquals;

import edu.wpi.first.util.PixelFormat;
import org.junit.jupiter.api.Test;

class VideoModeTest {
  @Test
  void equalityTest() {
    VideoMode a = new VideoMode(PixelFormat.kMJPEG, 1920, 1080, 30);
    VideoMode b = new VideoMode(PixelFormat.kMJPEG, 1920, 1080, 30);

    assertEquals(a, b);
    assertNotEquals(a, null);
  }
}
