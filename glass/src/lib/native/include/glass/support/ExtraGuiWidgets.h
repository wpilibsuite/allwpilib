// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

namespace glass {

class DataSource;

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

/**
 * Draw a 2D array of LEDs.
 *
 * Values are indices into colors array.  Positive values are filled (lit),
 * negative values are unfilled (dark / border only).  The actual color index
 * is the absolute value of the value - 1.  0 values are not drawn at all
 * (an empty space is left).
 *
 * @param values values array
 * @param sources sources array
 * @param numValues size of values and sources arrays
 * @param cols number of columns
 * @param colors colors array
 * @param size size of each LED (both horizontal and vertical);
 *             if 0, defaults to 1/2 of font size
 * @param spacing spacing between each LED (both horizontal and vertical);
 *                if 0, defaults to 1/3 of font size
 * @param config 2D array configuration
 */
void DrawLEDSources(const int* values, DataSource** sources, int numValues,
                    int cols, const ImU32* colors, float size = 0.0f,
                    float spacing = 0.0f,
                    const LEDConfig& config = LEDConfig{});

/**
 * Delete button (X in circle), based on ImGui::CloseButton().
 */
bool DeleteButton(ImGuiID id, const ImVec2& pos);

/**
 * Create a small overlapping delete button for collapsing headers.
 */
bool HeaderDeleteButton(const char* label);

/**
 * Settings button similar to ImGui::CloseButton.
 */
bool HamburgerButton(const ImGuiID id, const ImVec2 position);

/**
 * Edit a value with expression input. Similar to ImGui::InputScalar()
 */
template <typename V>
bool InputExpr(const char* label, V* v, const char* format,
               ImGuiInputTextFlags flags = 0);
extern template bool InputExpr(const char*, int64_t*, const char*,
                               ImGuiInputTextFlags);
extern template bool InputExpr(const char*, float*, const char*,
                               ImGuiInputTextFlags);
extern template bool InputExpr(const char*, double*, const char*,
                               ImGuiInputTextFlags);

}  // namespace glass
