// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.datalog;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.List;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.Timeout;
import org.junit.jupiter.api.io.TempDir;

class DataLogBackgroundWriterTest {
  @Test
  @Timeout(5)
  void transferPreservesRecords(@TempDir Path root) throws IOException, InterruptedException {
    Path source = Files.createDirectory(root.resolve("source"));
    Path dest = Files.createDirectory(root.resolve("dest"));
    int entry;
    try (var log = new DataLogBackgroundWriter(source.toString(), "test.wpilog", 30.0, "extra")) {
      entry = log.start("integer", "int64", "metadata", 1);
      log.appendInteger(entry, 42, 2);
      log.setDir(dest.toString(), 100000);
      while (!log.getDir().equals(dest.toString())) {
        Thread.sleep(10);
      }
      assertFalse(Files.exists(source.resolve("test.wpilog")));
      log.appendInteger(entry, 43, 3);
      log.setFilename("renamed.wpilog");
    }
    // Memory-mapped files prevent temporary directory cleanup on Windows.
    var reader =
        new DataLogReader(ByteBuffer.wrap(Files.readAllBytes(dest.resolve("renamed.wpilog"))));
    assertTrue(reader.isValid());
    assertEquals("extra", reader.getExtraHeader());
    var records = new ArrayList<DataLogRecord>();
    reader.forEach(records::add);
    var values = new ArrayList<Long>();
    int starts = 0;
    for (var record : records) {
      if (record.isStart()) {
        ++starts;
      }
      if (record.getEntry() == entry) {
        values.add(record.getInteger());
      }
    }
    assertEquals(1, starts);
    assertEquals(List.of(42L, 43L), values);
  }

  @Test
  void transferSizeLimit(@TempDir Path root) throws IOException {
    Path dest = Files.createDirectory(root.resolve("dest"));
    try (var log = new DataLogBackgroundWriter(root.toString(), "test.wpilog", 30.0)) {
      int entry = log.start("integer", "int64", "", 1);
      log.appendInteger(entry, 42, 2);
      assertThrows(IllegalArgumentException.class, () -> log.setDir(dest.toString(), -1));
      log.setDir(dest.toString(), 12);
    }
    var reader =
        new DataLogReader(ByteBuffer.wrap(Files.readAllBytes(root.resolve("test.wpilog"))));
    assertTrue(reader.isValid());
    assertFalse(Files.exists(dest.resolve("test.wpilog")));
  }
}
