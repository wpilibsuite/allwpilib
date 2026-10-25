// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.system;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.attribute.FileTime;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.Timeout;
import org.junit.jupiter.api.io.TempDir;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.CsvSource;
import org.wpilib.datalog.DataLogBackgroundWriter;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.simulation.AlertSim;
import org.wpilib.util.Alert;

class DataLogManagerTest {
  @Test
  @Timeout(5)
  void lateUsbStorage(@TempDir Path root) throws IOException, InterruptedException {
    Path source = Files.createDirectory(root.resolve("internal"));
    Path usb = root.resolve("usb");
    try (var log = new DataLogBackgroundWriter(source.toString(), "test.wpilog")) {
      int entry = log.start("integer", "int64", "", 1);
      log.appendInteger(entry, 42, 2);
      assertFalse(DataLogManager.tryTransferToUsb(log, usb));
      assertEquals(source.toString(), log.getDir());

      Files.createDirectory(usb);
      while (!DataLogManager.tryTransferToUsb(log, usb)) {
        Thread.sleep(10);
      }
      assertEquals(usb.resolve("logs").toString(), log.getDir());
      assertTrue(Files.exists(usb.resolve("logs/test.wpilog")));
      assertFalse(Files.exists(source.resolve("test.wpilog")));
    }
  }

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
            "WPILIB_20261022_123456_Event_T4.wpilog",
            "WPILIB_20261022_123456_CALA_P1.2.wpilog",
            "WPILIB_20261022_123456_WAMIL_Q123.12.wpilog",
            "WPILIB_20261022_123456_MNDU2_E13.3.wpilog",
            "WPILIB_20261022_123456_Event_T4.2.wpilog")) {
      assertTrue(DataLogManager.isMatchLog(filename), filename);
    }

    for (String filename :
        List.of(
            "WPILIB_20261022_123456.wpilog",
            "WPILIB_20261022_123456_Event_Q.wpilog",
            "WPILIB_20261022_123456_Event_Qabc.wpilog",
            "WPILIB_20261022_123456_Event_X3.wpilog",
            "WPILIB_TBD_1234567890abcdef.wpilog",
            "WPILIB_20261022_123456_Event_Q3.wpilog.bak",
            "WPILIB_20261022_123456_Event_Q.2.wpilog",
            "WPILIB_20261022_123456_Event_Q1..2.wpilog",
            "WPILIB_20261022_123456_Event_Q1.2.3.wpilog",
            "WPILIB_20261022_123456_Event_Q1.abc.wpilog",
            "WPILIB_20261022_123456_Event_Q1..wpilog",
            "WPILIB_20261022_123456.2.wpilog")) {
      assertFalse(DataLogManager.isMatchLog(filename), filename);
    }
  }

  @ParameterizedTest
  @CsvSource({
    "20, 10, 0, 10", // Protected logs count toward the minimum.
    "5, 10, 0, 5", // Retain ten permanent logs in total.
    "20, 10, 199999950, 1", // Stop once sufficient space is recovered.
    "20, 10, 200000000, 0", // No cleanup is needed.
    "9, 1, 0, 0" // Temporary and unrelated files do not count toward the minimum.
  })
  void cleanup(
      int matchCount, int nonMatchCount, long freeSpace, int deletedCount, @TempDir Path logDir)
      throws IOException {
    HAL.initialize();
    List<Path> matches = new ArrayList<>();
    for (int i = 0; i < matchCount; ++i) {
      matches.add(
          createLog(
              logDir,
              "WPILIB_20261022_123456_Event_Q" + i + (i % 2 == 0 ? "" : ".2") + ".wpilog",
              i));
    }
    List<Path> nonMatches = new ArrayList<>();
    for (int i = 0; i < nonMatchCount; ++i) {
      nonMatches.add(
          createLog(logDir, "WPILIB_20261022_1234%02d.wpilog".formatted(i), matchCount + i));
    }
    final Path temporary = createLog(logDir, "WPILIB_TBD_1234567890abcdef.wpilog", 100);
    final Path unrelated = createLog(logDir, "unrelated.wpilog", 101);

    DataLogManager.deleteOldLogs(logDir.toFile(), freeSpace);

    for (Path path : matches) {
      assertTrue(Files.exists(path), path.toString());
    }
    for (int i = 0; i < nonMatchCount; ++i) {
      assertEquals(
          i >= deletedCount, Files.exists(nonMatches.get(i)), nonMatches.get(i).toString());
    }
    assertTrue(Files.exists(temporary));
    assertTrue(Files.exists(unrelated));
  }

  private Path createLog(Path logDir, String filename, int order) throws IOException {
    Path path = Files.write(logDir.resolve(filename), new byte[100]);
    Files.setLastModifiedTime(path, FileTime.fromMillis(1_000L * order));
    return path;
  }

  @Test
  @Timeout(5)
  void alertLifetime(@TempDir Path logDir) throws InterruptedException {
    HAL.initialize();
    for (int i = 0; i < 2; ++i) {
      DataLogManager.start(logDir.toString());
      try {
        assertEquals(logDir.toString(), DataLogManager.getLogDir());
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
