// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

namespace wpi {

/**
 * Provides access to the Driver Station display.
 *
 * Line mode is the default display mode.
 */
class DriverStationDisplay final {
 public:
  DriverStationDisplay() = delete;

  /** Driver Station display mode. */
  enum class Mode {
    /** Display line mode. This is the default display mode. */
    Line,
    /** Raw ANSI text mode. */
    RawAnsi
  };

  /**
   * Sets the display mode.
   *
   * Mode::Line is the default display mode. Setting Mode::Line clears any
   * pending display lines. Setting Mode::RawAnsi clears the display.
   *
   * @param mode display mode
   */
  static void SetMode(Mode mode);

  /**
   * Adds display data in line mode.
   *
   * Repeated calls with the same caption before UpdateLines() replace the
   * previous line. The caption is used to identify the line and is not
   * displayed. Empty or whitespace-only captions always append a new line.
   *
   * @param caption Line caption.
   * @param line Line contents.
   */
  static void AddData(std::string_view caption, std::string_view line);

  /**
   * Adds an uncaptioned display line in line mode.
   *
   * This is equivalent to calling AddData() with an empty caption, which always
   * appends a new line.
   *
   * @param line Line contents.
   */
  static void AddLine(std::string_view line);

  /**
   * Updates the display with all pending lines and clears the pending lines.
   *
   * Updates are sent at most once every 230 ms. If called before 230 ms has
   * elapsed since the last update, the pending lines are cleared without
   * sending.
   */
  static void UpdateLines();

  /**
   * Writes ANSI text directly to the display in raw ANSI mode.
   *
   * This call is ignored unless the display is in raw ANSI mode.
   *
   * @param data ANSI text to write.
   */
  static void WriteRawAnsiText(std::string_view data);

  /**
   * Clears the display in raw ANSI mode.
   *
   * This call is ignored unless the display is in raw ANSI mode.
   */
  static void ClearRaw();
};

}  // namespace wpi
