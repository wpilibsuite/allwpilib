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
 */
void DrawLEDs(int* values, int numValues, int cols, const ImU32* colors,
              float size = 0.0f, float spacing = 0.0f);

}  // namespace halsimgui
