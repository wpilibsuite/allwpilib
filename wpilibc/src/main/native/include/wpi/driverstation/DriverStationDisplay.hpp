// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include "wpi/hal/DriverStation.h"
#include "wpi/util/string.hpp"

namespace wpi {

/**
 * Provides access to the Driver Station display.
 *
 * Line mode is the default display mode.
 */
class DriverStationDisplay final {
 public:
  DriverStationDisplay() = delete;

  /**
   * Sets the display to line mode and clears any pending display lines.
   *
   * Line mode is the default display mode.
   */
  static void SetLineMode() {
    HAL_SetDisplayLineMode();
  }

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
  static void AddData(std::string_view caption, std::string_view line) {
    WPI_String captionWpiStr = wpi::util::make_string(caption);
    WPI_String lineWpiStr = wpi::util::make_string(line);
    HAL_SetDisplayLine(&captionWpiStr, &lineWpiStr);
  }

  /**
   * Adds an uncaptioned display line in line mode.
   *
   * This is equivalent to calling AddData() with an empty caption, which always
   * appends a new line.
   *
   * @param line Line contents.
   */
  static void AddLine(std::string_view line) {
    AddData({}, line);
  }

  /**
   * Updates the display with all pending lines and clears the pending lines.
   *
   * Updates are sent at most once every 230 ms. If called before 230 ms has
   * elapsed since the last update, the pending lines are cleared without
   * sending.
   */
  static void UpdateLines() {
    HAL_UpdateDisplayLines();
  }

  /**
   * Sets the display to raw mode and clears the display.
   */
  static void SetRawMode() {
    HAL_SetDisplayRawMode();
  }

  /**
   * Writes ANSI text directly to the display in raw mode.
   *
   * This call is ignored unless the display is in raw mode.
   *
   * @param data ANSI text to write.
   */
  static void WriteRawAnsiText(std::string_view data) {
    WPI_String dataWpiStr = wpi::util::make_string(data);
    HAL_WriteDisplayAnsiText(&dataWpiStr);
  }

  /**
   * Clears the display in raw mode.
   *
   * This call is ignored unless the display is in raw mode.
   */
  static void ClearRaw() {
    HAL_ClearDisplay();
  }
};

}  // namespace wpi
