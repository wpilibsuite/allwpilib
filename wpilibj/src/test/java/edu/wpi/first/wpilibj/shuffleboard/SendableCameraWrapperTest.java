// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.shuffleboard;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

import edu.wpi.first.networktables.NetworkTableInstance;
import org.junit.jupiter.api.AfterAll;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

class SendableCameraWrapperTest {
  @BeforeEach
  void setup() {
    NetworkTableInstance.getDefault().deleteAllEntries();
    SendableCameraWrapper.clearWrappers();
  }

  @AfterAll
  static void tearDown() {
    NetworkTableInstance.getDefault().deleteAllEntries();
  }

  @Test
  void testNullCameraName() {
    assertThrows(NullPointerException.class, () -> SendableCameraWrapper.wrap(null, ""));
  }

  @Test
  void testEmptyCameraName() {
    assertThrows(IllegalArgumentException.class, () -> SendableCameraWrapper.wrap("", ""));
  }

  @Test
  void testNullUrlArray() {
    assertThrows(
        NullPointerException.class, () -> SendableCameraWrapper.wrap("name", (String[]) null));
  }

  @Test
  void testNullUrlInArray() {
    assertThrows(NullPointerException.class, () -> SendableCameraWrapper.wrap("name", "url", null));
  }

  @Test
  void testEmptyUrlArray() {
    assertThrows(IllegalArgumentException.class, () -> SendableCameraWrapper.wrap("name"));
  }

  @Test
  void testUrlsAddedToNt() {
    SendableCameraWrapper.wrap("name", "url1", "url2");
    assertArrayEquals(
        new String[] {"url1", "url2"},
        NetworkTableInstance.getDefault()
            .getEntry("/CameraPublisher/name/streams")
            .getValue()
            .getStringArray());
  }
}
