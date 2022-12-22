// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.apriltag;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

class AprilTagDetectorTest {
  @SuppressWarnings("MemberName")
  AprilTagDetector detector;

  @BeforeEach
  void beforeEach() {
    detector = new AprilTagDetector();
  }

  @AfterEach
  void afterEach() {
    detector.close();
  }

  @Test
  void testNumThreads() {
    assertEquals(detector.getNumThreads(), 1);
    detector.setNumThreads(2);
    assertEquals(detector.getNumThreads(), 2);
  }

  @Test
  void testAdd16h5() {
    assertDoesNotThrow(() -> detector.addFamily("tag16h5"));
    // duplicate addition is also okay
    assertDoesNotThrow(() -> detector.addFamily("tag16h5"));
  }

  @Test
  void testAdd25h9() {
    assertDoesNotThrow(() -> detector.addFamily("tag25h9"));
  }

  @Test
  void testAdd36h11() {
    assertDoesNotThrow(() -> detector.addFamily("tag36h11"));
  }

  @Test
  void testAddMultiple() {
    assertDoesNotThrow(() -> detector.addFamily("tag16h5"));
    assertDoesNotThrow(() -> detector.addFamily("tag36h11"));
  }

  @Test
  void testRemoveFamily() {
    // okay to remove non-existent family
    detector.removeFamily("tag16h5");

    // add and remove
    detector.addFamily("tag16h5");
    detector.removeFamily("tag16h5");
  }
}
