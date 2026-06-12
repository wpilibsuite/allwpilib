// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.driverstation;

import org.wpilib.hardware.hal.DriverStationJNI;

/**
 * Provides access to the Driver Station display.
 *
 * <p>Line mode is the default display mode.
 */
public final class DriverStationDisplay {
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
    switch (mode) {
      case Line -> DriverStationJNI.setDisplayLineMode();
      case RawAnsi -> DriverStationJNI.setDisplayRawMode();
      default -> throw new IllegalArgumentException("Unknown display mode: " + mode);
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
    DriverStationJNI.addDisplayLine(caption, line);
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
    DriverStationJNI.updateDisplayLines();
  }

  /**
   * Writes ANSI text directly to the display in raw ANSI mode.
   *
   * <p>This call is ignored unless the display is in raw ANSI mode.
   *
   * @param data ANSI text to write.
   */
  public static void writeRawAnsiText(String data) {
    DriverStationJNI.writeDisplayAnsiText(data);
  }

  /**
   * Clears the display in raw ANSI mode.
   *
   * <p>This call is ignored unless the display is in raw ANSI mode.
   */
  public static void clearRaw() {
    DriverStationJNI.clearDisplay();
  }
}
