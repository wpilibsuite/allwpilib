// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.datalog;

import java.io.ByteArrayOutputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.concurrent.CountDownLatch;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.io.TempDir;

class FileLoggerTest {
  @Test
  void closeIsIdempotent(@TempDir Path tempDir) throws Exception {
    Path file = tempDir.resolve("console.txt");
    Files.createFile(file);

    try (DataLogWriter log = new DataLogWriter(new ByteArrayOutputStream())) {
      FileLogger logger = new FileLogger(file.toString(), log, "console");
      logger.close();
      logger.close();
    }
  }

  @Test
  void closeIsThreadSafe(@TempDir Path tempDir) throws Exception {
    Path file = tempDir.resolve("console.txt");
    Files.createFile(file);

    try (DataLogWriter log = new DataLogWriter(new ByteArrayOutputStream())) {
      FileLogger logger = new FileLogger(file.toString(), log, "console");
      CountDownLatch ready = new CountDownLatch(2);
      CountDownLatch start = new CountDownLatch(1);
      Thread first = closeOnSignal(logger, ready, start);
      final Thread second = closeOnSignal(logger, ready, start);

      ready.await();
      start.countDown();
      first.join();
      second.join();
    }
  }

  private static Thread closeOnSignal(
      FileLogger logger, CountDownLatch ready, CountDownLatch start) {
    Thread thread =
        new Thread(
            () -> {
              ready.countDown();
              try {
                start.await();
              } catch (InterruptedException ex) {
                Thread.currentThread().interrupt();
              }
              logger.close();
            });
    thread.start();
    return thread;
  }
}
