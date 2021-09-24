// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.cscore;

import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTimeoutPreemptively;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.time.Duration;
import java.util.Arrays;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;
import org.junit.jupiter.api.Nested;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.condition.EnabledOnOs;
import org.junit.jupiter.api.condition.OS;

class UsbCameraTest {
  @Nested
  @EnabledOnOs(OS.LINUX)
  static class ConnectVerbose {
    @Test
    void setConnectVerboseEnabledTest() {
      try (UsbCamera camera = new UsbCamera("Nonexistant Camera", getNonexistentCameraDev())) {
        camera.setConnectVerbose(1);

        CompletableFuture<String> result = new CompletableFuture<>();
        CameraServerJNI.setLogger((level, file, line, message) -> result.complete(message), 20);

        assertTimeoutPreemptively(
            Duration.ofSeconds(5),
            () -> assertTrue(result.get().contains("Connecting to USB camera on ")));
      }
    }

    @Test
    void setConnectVerboseDisabledTest() {
      try (UsbCamera camera = new UsbCamera("Nonexistant Camera", getNonexistentCameraDev())) {
        camera.setConnectVerbose(0);

        CompletableFuture<String> result = new CompletableFuture<>();
        CameraServerJNI.setLogger((level, file, line, message) -> result.complete(message), 20);

        assertThrows(TimeoutException.class, () -> result.get(3, TimeUnit.SECONDS));
      }
    }
  }

  private static int getNonexistentCameraDev() {
    return Arrays.stream(CameraServerJNI.enumerateUsbCameras())
            .mapToInt(info -> info.dev)
            .max()
            .orElse(-1)
        + 20;
  }
}
