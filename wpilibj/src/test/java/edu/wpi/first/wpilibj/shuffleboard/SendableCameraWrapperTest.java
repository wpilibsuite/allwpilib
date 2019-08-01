/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

import org.junit.jupiter.api.AfterAll;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import edu.wpi.first.networktables.NetworkTableInstance;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

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
    assertThrows(NullPointerException.class,
        () -> SendableCameraWrapper.wrap(null, ""));
  }

  @Test
  void testEmptyCameraName() {
    assertThrows(IllegalArgumentException.class,
        () -> SendableCameraWrapper.wrap("", ""));
  }

  @Test
  void testNullUrlArray() {
    assertThrows(NullPointerException.class,
        () -> SendableCameraWrapper.wrap("name", (String[]) null));
  }

  @Test
  void testNullUrlInArray() {
    assertThrows(NullPointerException.class,
        () -> SendableCameraWrapper.wrap("name", "url", null));
  }

  @Test
  void testEmptyUrlArray() {
    assertThrows(IllegalArgumentException.class,
        () -> SendableCameraWrapper.wrap("name"));
  }

  @Test
  void testUrlsAddedToNt() {
    SendableCameraWrapper.wrap("name", "url1", "url2");
    assertArrayEquals(
        new String[]{"url1", "url2"},
        NetworkTableInstance.getDefault()
            .getEntry("/CameraPublisher/name/streams").getValue().getStringArray()
    );
  }
}
