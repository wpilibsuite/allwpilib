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
inline constexpr Vector2d kAppWindowSize{1280, 720};

// Menubar height
inline constexpr int kMenubarHeight = 20;

// Gap between window edges
inline constexpr int kWindowGap = 5;

// Left column position and size
inline constexpr Vector2d kLeftColPos{kWindowGap, kMenubarHeight + kWindowGap};
inline constexpr Vector2d kLeftColSize{
    310, kAppWindowSize.y - kLeftColPos.y - kWindowGap};

// Left column contents
inline constexpr Vector2d kLogLoaderWindowPos = kLeftColPos;
inline constexpr Vector2d kLogLoaderWindowSize{kLeftColSize.x, 450};
inline constexpr Vector2d kDataSelectorWindowPos =
    kLogLoaderWindowPos + Vector2d{0, kLogLoaderWindowSize.y + kWindowGap};
inline constexpr Vector2d kDataSelectorWindowSize{
    kLeftColSize.x, kAppWindowSize.y - kWindowGap - kDataSelectorWindowPos.y};

// Center column position and size
inline constexpr Vector2d kCenterColPos =
    kLeftColPos + Vector2d{kLeftColSize.x + kWindowGap, 0};
inline constexpr Vector2d kCenterColSize{
    360, kAppWindowSize.y - kLeftColPos.y - kWindowGap};

// Center column contents
inline constexpr Vector2d kAnalyzerWindowPos = kCenterColPos;
inline constexpr Vector2d kAnalyzerWindowSize{kCenterColSize.x, 550};
inline constexpr Vector2d kProgramLogWindowPos =
    kAnalyzerWindowPos + Vector2d{0, kAnalyzerWindowSize.y + kWindowGap};
inline constexpr Vector2d kProgramLogWindowSize{
    kCenterColSize.x, kAppWindowSize.y - kWindowGap - kProgramLogWindowPos.y};

// Right column position and size
inline constexpr Vector2d kRightColPos =
    kCenterColPos + Vector2d{kCenterColSize.x + kWindowGap, 0};
inline constexpr Vector2d kRightColSize =
    kAppWindowSize - kRightColPos - Vector2d{kWindowGap, kWindowGap};

// Right column contents
inline constexpr Vector2d kDiagnosticPlotWindowPos = kRightColPos;
inline constexpr Vector2d kDiagnosticPlotWindowSize = kRightColSize;

// Text box width as a multiple of the font size
inline constexpr int kTextBoxWidthMultiple = 10;
}  // namespace sysid
