// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.driverstation;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
import org.wpilib.hardware.hal.DriverStationJNI;

/**
 * Provides access to the Driver Station display.
 *
 * <p>Line mode is the default display mode.
 */
public final class DriverStationDisplay {
  private static final long UPDATE_PERIOD_NANOS = 230_000_000L;
  private static final String CLEAR_DISPLAY = "\033[2J\033[H";

  private static final Lock m_displayLock = new ReentrantLock();
  private static boolean rawMode;
  private static final Map<String, Integer> lineMap = new HashMap<>();
  private static final List<String> lines = new ArrayList<>();
  private static long lastDisplayUpdate = System.nanoTime() - UPDATE_PERIOD_NANOS;

  private DriverStationDisplay() {}

  /** Driver Station display mode. */
  public enum Mode {
    /** Display line mode. This is the default display mode. */
    Line,

    /** Raw ANSI text mode. */
    RawAnsi
  }

  /**
   * Sets the display mode.
   *
   * <p>{@link Mode#Line} is the default display mode. Setting {@link Mode#Line} clears any pending
   * display lines. Setting {@link Mode#RawAnsi} clears the display.
   *
   * @param mode display mode
   */
  public static void setMode(Mode mode) {
    m_displayLock.lock();
    try {
      switch (mode) {
        case Line -> {
          rawMode = false;
          lineMap.clear();
          lines.clear();
        }
        case RawAnsi -> {
          rawMode = true;
          DriverStationJNI.writeDisplayAnsiText(CLEAR_DISPLAY);
        }
        default -> throw new IllegalArgumentException("Unknown display mode: " + mode);
      }
    } finally {
      m_displayLock.unlock();
    }
  }

  /**
   * Adds display data in line mode.
   *
   * <p>Repeated calls with the same caption before {@link #updateLines()} replace the previous
   * line. The caption is used to identify the line and is not displayed. Empty or whitespace-only
   * captions always append a new line.
   *
   * @param caption Line caption.
   * @param line Line contents.
   */
  public static void addData(String caption, String line) {
    m_displayLock.lock();
    try {
      addDataUnderLock(caption, line);
    } finally {
      m_displayLock.unlock();
    }
  }

  /**
   * Adds display data in line mode.
   *
   * <p>Repeated calls with the same caption before {@link #updateLines()} replace the previous
   * line. The caption is used to identify the line and is not displayed. Empty or whitespace-only
   * captions always append a new line.
   *
   * <p>The value is converted to text with {@link String#valueOf(Object)}.
   *
   * @param caption Line caption.
   * @param value Line value.
   */
  public static void addData(String caption, Object value) {
    addData(caption, String.valueOf(value));
  }

  /**
   * Adds formatted display data in line mode.
   *
   * <p>Repeated calls with the same caption before {@link #updateLines()} replace the previous
   * line. The caption is used to identify the line and is not displayed. Empty or whitespace-only
   * captions always append a new line.
   *
   * @param caption Line caption.
   * @param format Format string.
   * @param args Format arguments.
   */
  public static void addData(String caption, String format, Object... args) {
    addData(caption, String.format(format, args));
  }

  private static void addDataUnderLock(String caption, String line) {
    if (rawMode) {
      return;
    }

    String captionText = nonNull(caption);
    String lineText = nonNull(line);

    if (captionText.isBlank()) {
      lines.add(lineText);
      return;
    }

    Integer lineNum = lineMap.get(captionText);
    if (lineNum == null) {
      lineMap.put(captionText, lines.size());
      lines.add(lineText);
    } else if (lineNum < lines.size()) {
      lines.set(lineNum, lineText);
    }
  }

  /**
   * Adds an uncaptioned display line in line mode.
   *
   * <p>This is equivalent to calling {@link #addData(String, String)} with an empty caption, which
   * always appends a new line.
   *
   * @param line Line contents.
   */
  public static void addLine(String line) {
    addData("", line);
  }

  /**
   * Updates the display with all pending lines and clears the pending lines.
   *
   * <p>Updates are sent at most once every 230 ms. If called before 230 ms has elapsed since the
   * last update, the pending lines are cleared without sending.
   */
  public static void updateLines() {
    m_displayLock.lock();
    try {
      if (rawMode) {
        return;
      }

      long now = System.nanoTime();
      if (now - lastDisplayUpdate < UPDATE_PERIOD_NANOS) {
        lineMap.clear();
        lines.clear();
        return;
      }
      lastDisplayUpdate = now;

      StringBuilder builder = new StringBuilder();
      builder.append(CLEAR_DISPLAY);
      for (String line : lines) {
        builder.append(line).append("\033[0m\n");
      }

      DriverStationJNI.writeDisplayAnsiText(builder.toString());

      lineMap.clear();
      lines.clear();
    } finally {
      m_displayLock.unlock();
    }
  }

  /**
   * Writes ANSI text directly to the display in raw ANSI mode.
   *
   * <p>This call is ignored unless the display is in raw ANSI mode.
   *
   * @param data ANSI text to write.
   */
  public static void writeRawAnsiText(String data) {
    m_displayLock.lock();
    try {
      if (rawMode) {
        DriverStationJNI.writeDisplayAnsiText(nonNull(data));
      }
    } finally {
      m_displayLock.unlock();
    }
  }

  /**
   * Clears the display in raw ANSI mode.
   *
   * <p>This call is ignored unless the display is in raw ANSI mode.
   */
  public static void clearRaw() {
    m_displayLock.lock();
    try {
      if (rawMode) {
        DriverStationJNI.writeDisplayAnsiText(CLEAR_DISPLAY);
      }
    } finally {
      m_displayLock.unlock();
    }
  }

  private static String nonNull(String value) {
    return value == null ? "" : value;
  }
}
