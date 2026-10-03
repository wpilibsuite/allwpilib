// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <imgui.h>

namespace sysid {
/**
 * constexpr shim for ImVec2.
 */
struct Vector2d {
  /**
   * X coordinate.
   */
  float x = 0;

  /**
   * Y coordinate.
   */
  float y = 0;

  /**
   * Vector2d addition operator.
   *
   * @param rhs Vector to add.
   * @return Sum of two vectors.
   */
  constexpr Vector2d operator+(const Vector2d& rhs) const {
    return Vector2d{x + rhs.x, y + rhs.y};
  }

  /**
   * Vector2d subtraction operator.
   *
   * @param rhs Vector to subtract.
   * @return Difference of two vectors.
   */
  constexpr Vector2d operator-(const Vector2d& rhs) const {
    return Vector2d{x - rhs.x, y - rhs.y};
  }

  /**
   * Conversion operator to ImVec2.
   */
  explicit operator ImVec2() const { return ImVec2{x, y}; }
};

// App window size
inline constexpr Vector2d APP_WINDOW_SIZE{1280, 720};

// Menubar height
inline constexpr int MENUBAR_HEIGHT = 20;

// Gap between window edges
inline constexpr int WINDOW_GAP = 5;

// Left column position and size
inline constexpr Vector2d LEFT_COL_POS{WINDOW_GAP, MENUBAR_HEIGHT + WINDOW_GAP};
inline constexpr Vector2d LEFT_COL_SIZE{
    310, APP_WINDOW_SIZE.y - LEFT_COL_POS.y - WINDOW_GAP};

// Left column contents
inline constexpr Vector2d LOG_LOADER_WINDOW_POS = LEFT_COL_POS;
inline constexpr Vector2d LOG_LOADER_WINDOW_SIZE{LEFT_COL_SIZE.x, 450};
inline constexpr Vector2d DATA_SELECTOR_WINDOW_POS =
    LOG_LOADER_WINDOW_POS + Vector2d{0, LOG_LOADER_WINDOW_SIZE.y + WINDOW_GAP};
inline constexpr Vector2d DATA_SELECTOR_WINDOW_SIZE{
    LEFT_COL_SIZE.x,
    APP_WINDOW_SIZE.y - WINDOW_GAP - DATA_SELECTOR_WINDOW_POS.y};

// Center column position and size
inline constexpr Vector2d CENTER_COL_POS =
    LEFT_COL_POS + Vector2d{LEFT_COL_SIZE.x + WINDOW_GAP, 0};
inline constexpr Vector2d CENTER_COL_SIZE{
    360, APP_WINDOW_SIZE.y - LEFT_COL_POS.y - WINDOW_GAP};

// Center column contents
inline constexpr Vector2d ANALYZER_WINDOW_POS = CENTER_COL_POS;
inline constexpr Vector2d ANALYZER_WINDOW_SIZE{CENTER_COL_SIZE.x, 550};
inline constexpr Vector2d PROGRAM_LOG_WINDOW_POS =
    ANALYZER_WINDOW_POS + Vector2d{0, ANALYZER_WINDOW_SIZE.y + WINDOW_GAP};
inline constexpr Vector2d PROGRAM_LOG_WINDOW_SIZE{
    CENTER_COL_SIZE.x,
    APP_WINDOW_SIZE.y - WINDOW_GAP - PROGRAM_LOG_WINDOW_POS.y};

// Right column position and size
inline constexpr Vector2d RIGHT_COL_POS =
    CENTER_COL_POS + Vector2d{CENTER_COL_SIZE.x + WINDOW_GAP, 0};
inline constexpr Vector2d RIGHT_COL_SIZE =
    APP_WINDOW_SIZE - RIGHT_COL_POS - Vector2d{WINDOW_GAP, WINDOW_GAP};

// Right column contents
inline constexpr Vector2d DIAGNOSTIC_PLOT_WINDOW_POS = RIGHT_COL_POS;
inline constexpr Vector2d DIAGNOSTIC_PLOT_WINDOW_SIZE = RIGHT_COL_SIZE;

// Text box width as a multiple of the font size
inline constexpr int TEXT_BOX_WIDTH_MULTIPLE = 10;
}  // namespace sysid
