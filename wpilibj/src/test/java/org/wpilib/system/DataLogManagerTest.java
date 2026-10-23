// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.system;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.nio.file.Path;
import java.util.Arrays;
import java.util.List;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.Timeout;
import org.junit.jupiter.api.io.TempDir;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.simulation.AlertSim;
import org.wpilib.util.Alert;

class DataLogManagerTest {
  private List<AlertSim.AlertInfo> getStorageAlerts() {
    return Arrays.stream(AlertSim.getAll())
        .filter(alert -> "DataLogManager".equals(alert.group))
        .toList();
  }

  @Test
  void matchLogFilenameDetection() {
    for (String filename :
        List.of(
            "WPILIB_20261022_123456_CALA_P1.wpilog",
            "WPILIB_20261022_123456_WAMIL_Q123.wpilog",
            "WPILIB_20261022_123456_MNDU2_E13.wpilog",
            "WPILIB_20261022_123456_Event_T4.wpilog")) {
      assertTrue(DataLogManager.isMatchLog(filename), filename);
    }

    for (String filename :
        List.of(
            "WPILIB_20261022_123456.wpilog",
            "WPILIB_20261022_123456_Event_Q.wpilog",
            "WPILIB_20261022_123456_Event_Qabc.wpilog",
            "WPILIB_20261022_123456_Event_X3.wpilog",
            "WPILIB_20261022_123456_Event_Q3.wpilog.bak",
            "WPILIB_TBD_1234567890abcdef.wpilog")) {
      assertFalse(DataLogManager.isMatchLog(filename), filename);
    }
  }

  @Test
  @Timeout(5)
  void alertLifetime(@TempDir Path logDir) throws InterruptedException {
    HAL.initialize();
    for (int i = 0; i < 2; ++i) {
      DataLogManager.start(logDir.toString());
      try {
        for (int attempt = 0; attempt < 100 && getStorageAlerts().size() < 2; ++attempt) {
          Thread.sleep(10);
        }
        var alerts = getStorageAlerts();
        assertEquals(2, alerts.size());
        for (var alert : alerts) {
          if ("internalStorage".equals(alert.id)) {
            assertFalse(alert.isActive());
            assertEquals(Alert.Level.LOW, alert.level);
          } else {
            assertEquals("lowSpace", alert.id);
            assertEquals(Alert.Level.MEDIUM, alert.level);
          }
        }
      } finally {
        DataLogManager.stop();
      }
      assertTrue(getStorageAlerts().isEmpty());
    }
  }

  @Test
  void freeSpaceAlert() {
    HAL.initialize();
    try (var alert = new Alert("DataLogManagerTest", "lowSpace", "", Alert.Level.MEDIUM)) {
      // Include both sides of the warning and cleanup thresholds, then recover.
      for (long freeSpace :
          new long[] {
            500000000, 400000000, 399999999, 200000000, 199999999, 0, 100000000, 400000000
          }) {
        DataLogManager.updateFreeSpaceAlert(alert, freeSpace);
        assertEquals(freeSpace < 400000000, alert.get());
        if (alert.get()) {
          assertTrue(alert.getText().contains(freeSpace / 1000000 + " MB of free space remaining"));
          assertTrue(alert.getText().contains("at startup below 200 MB"));
        }
      }
    }
  }
}
