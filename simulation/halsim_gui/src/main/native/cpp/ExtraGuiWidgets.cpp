/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ExtraGuiWidgets.h"

namespace halsimgui {

void DrawLEDs(const int* values, int numValues, int cols, const ImU32* colors,
              float size, float spacing, const LEDConfig& config) {
  if (numValues == 0 || cols < 1) return;
  if (size == 0) size = ImGui::GetFontSize() / 2.0;
  if (spacing == 0) spacing = ImGui::GetFontSize() / 3.0;

  int rows = (numValues + cols - 1) / cols;
  float inc = size + spacing;

  ImDrawList* drawList = ImGui::GetWindowDrawList();
  const ImVec2 p = ImGui::GetCursorScreenPos();

  float ystart, yinc;
  if (config.start & 1) {
    // lower
    ystart = p.y + size / 2 + inc * (rows - 1);
    yinc = -inc;
  } else {
    // upper
    ystart = p.y + size / 2;
    yinc = inc;
  }

  float xstart, xinc;
  if (config.start & 2) {
    // right
    xstart = p.x + size / 2 + inc * (cols - 1);
    xinc = -inc;
  } else {
    // left
    xstart = p.x + size / 2;
    xinc = inc;
  }

  float x = xstart, y = ystart;
  if (config.order == LEDConfig::RowMajor) {
    // row major
    int row = 1;
    for (int i = 0; i < numValues; ++i) {
      if (i >= (row * cols)) {
        ++row;
        if (config.serpentine) {
          x -= xinc;
          xinc = -xinc;
        } else {
          x = xstart;
        }
        y += yinc;
      }
      if (values[i] > 0)
        drawList->AddRectFilled(ImVec2(x, y), ImVec2(x + size, y + size),
                                colors[values[i] - 1]);
      else if (values[i] < 0)
        drawList->AddRect(ImVec2(x, y), ImVec2(x + size, y + size),
                          colors[-values[i] - 1], 0.0f, 0, 1.0);
      x += xinc;
    }
  } else {
    // column major
    int col = 1;
    for (int i = 0; i < numValues; ++i) {
      if (i >= (col * rows)) {
        ++col;
        if (config.serpentine) {
          y -= yinc;
          yinc = -yinc;
        } else {
          y = ystart;
        }
        x += xinc;
      }
      if (values[i] > 0)
        drawList->AddRectFilled(ImVec2(x, y), ImVec2(x + size, y + size),
                                colors[values[i] - 1]);
      else if (values[i] < 0)
        drawList->AddRect(ImVec2(x, y), ImVec2(x + size, y + size),
                          colors[-values[i] - 1], 0.0f, 0, 1.0);
      y += yinc;
    }
  }

  ImGui::Dummy(ImVec2(inc * cols, inc * rows));
}

}  // namespace halsimgui
