/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <imgui.h>

namespace halsimgui {

/**
 * DrawLEDs() configuration for 2D arrays.
 */
struct LEDConfig {
  /**
   * Whether the major order is serpentined (e.g. the first row goes left to
   * right, the second row right to left, the third row left to right, and so
   * on).
   */
  bool serpentine = false;

  /**
   * The input array order (row-major or column-major).
   */
  enum Order { RowMajor = 0, ColumnMajor } order = RowMajor;

  /**
   * The starting location of the array (0 location).
   */
  enum Start {
    UpperLeft = 0,
    LowerLeft,
    UpperRight,
    LowerRight
  } start = UpperLeft;
};

/**
 * Draw a 2D array of LEDs.
 *
 * Values are indices into colors array.  Positive values are filled (lit),
 * negative values are unfilled (dark / border only).  The actual color index
 * is the absolute value of the value - 1.  0 values are not drawn at all
 * (an empty space is left).
 *
 * @param values values array
 * @param numValues size of values array
 * @param cols number of columns
 * @param colors colors array
 * @param size size of each LED (both horizontal and vertical);
 *             if 0, defaults to 1/2 of font size
 * @param spacing spacing between each LED (both horizontal and vertical);
 *                if 0, defaults to 1/3 of font size
 * @param config 2D array configuration
 */
void DrawLEDs(const int* values, int numValues, int cols, const ImU32* colors,
              float size = 0.0f, float spacing = 0.0f,
              const LEDConfig& config = LEDConfig{});

}  // namespace halsimgui
