/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.cscore;

import java.time.Duration;
import java.util.Arrays;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;

import org.junit.jupiter.api.Nested;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.condition.EnabledOnOs;
import org.junit.jupiter.api.condition.OS;

import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTimeoutPreemptively;
import static org.junit.jupiter.api.Assertions.assertTrue;

class UsbCameraTest {
  @Nested
  @EnabledOnOs(OS.LINUX)
  static class ConnectVerbose {
    @Test
    void setConnectVerboseEnabledTest() {
      try (UsbCamera camera = new UsbCamera("Nonexistant Camera", getNonexistentCameraDev())) {
        camera.setConnectVerbose(1);

        CompletableFuture<String> result = new CompletableFuture<>();
        CameraServerJNI.setLogger((level, file, line, message) -> {
          result.complete(message);
        }, 20);

        assertTimeoutPreemptively(Duration.ofSeconds(5),
            () -> assertTrue(result.get().contains("Connecting to USB camera on ")));
      }
    }

    @Test
    void setConnectVerboseDisabledTest() {
      try (UsbCamera camera = new UsbCamera("Nonexistant Camera", getNonexistentCameraDev())) {
        camera.setConnectVerbose(0);

        CompletableFuture<String> result = new CompletableFuture<>();
        CameraServerJNI.setLogger((level, file, line, message) -> {
          result.complete(message);
        }, 20);

        assertThrows(TimeoutException.class,
            () -> result.get(3, TimeUnit.SECONDS));
      }
    }
  }

  private static int getNonexistentCameraDev() {
    return Arrays.stream(CameraServerJNI.enumerateUsbCameras())
        .mapToInt(info -> info.dev)
        .max().orElseGet(() -> -1) + 1;
  }
}
