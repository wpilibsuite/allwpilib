// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.driverstation;

import org.wpilib.hardware.hal.DriverStationJNI;

/** Provides access to the Driver Station display. */
public final class DriverStationDisplay {
  private DriverStationDisplay() {}

  /** Sets the display to line mode and clears any pending display lines. */
  public static void setLineMode() {
    DriverStationJNI.setDisplayLineMode();
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
    DriverStationJNI.setDisplayLine(caption, line);
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

  /** Sets the display to raw mode and clears the display. */
  public static void setRawMode() {
    DriverStationJNI.setDisplayRawMode();
  }

  /**
   * Writes ANSI text directly to the display in raw mode.
   *
   * <p>This call is ignored unless the display is in raw mode.
   *
   * @param data ANSI text to write.
   */
  public static void writeRawAnsiText(String data) {
    DriverStationJNI.writeDisplayAnsiText(data);
  }

  /**
   * Clears the display in raw mode.
   *
   * <p>This call is ignored unless the display is in raw mode.
   */
  public static void clearRaw() {
    DriverStationJNI.clearDisplay();
  }
}
