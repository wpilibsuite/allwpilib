// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.net;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;

import org.junit.jupiter.api.Test;

class UsbDeviceDetectorTest {
  @Test
  void validatesPort() {
    assertThrows(
        IllegalArgumentException.class,
        () -> new UsbDeviceDetector(-1, device -> {}, device -> {}));
    assertThrows(
        IllegalArgumentException.class, () -> new UsbDeviceDetector(4, device -> {}, device -> {}));
  }

  @Test
  void desktopHasNoImplementation() {
    try (UsbDeviceDetector detector = new UsbDeviceDetector(0, device -> {}, device -> {})) {
      assertFalse(detector.hasImplementation());
      assertFalse(detector.start());
      assertFalse(detector.isRunning());
      detector.stop();
    }
  }
}
