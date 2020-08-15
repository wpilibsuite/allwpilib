/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ExtraGuiWidgets.h"

#include "GuiDataSource.h"

namespace halsimgui {

void DrawLEDSources(const int* values, GuiDataSource** sources, int numValues,
                    int cols, const ImU32* colors, float size, float spacing,
                    const LEDConfig& config) {
  if (numValues == 0 || cols < 1) return;
  if (size == 0) size = ImGui::GetFontSize() / 2.0;
  if (spacing == 0) spacing = ImGui::GetFontSize() / 3.0;

  int rows = (numValues + cols - 1) / cols;
  float inc = size + spacing;

  ImDrawList* drawList = ImGui::GetWindowDrawList();
  const ImVec2 p = ImGui::GetCursorScreenPos();

  float sized2 = size / 2;
  float ystart, yinc;
  if (config.start & 1) {
    // lower
    ystart = p.y + sized2 + inc * (rows - 1);
    yinc = -inc;
  } else {
    // upper
    ystart = p.y + sized2;
    yinc = inc;
  }

  float xstart, xinc;
  if (config.start & 2) {
    // right
    xstart = p.x + sized2 + inc * (cols - 1);
    xinc = -inc;
  } else {
    // left
    xstart = p.x + sized2;
    xinc = inc;
  }

  float x = xstart, y = ystart;
  int rowcol = 1;  // row for row-major, column for column-major
  for (int i = 0; i < numValues; ++i) {
    if (config.order == LEDConfig::RowMajor) {
      if (i >= (rowcol * cols)) {
        ++rowcol;
        if (config.serpentine) {
          x -= xinc;
          xinc = -xinc;
        } else {
          x = xstart;
        }
        y += yinc;
      }
    } else {
      if (i >= (rowcol * rows)) {
        ++rowcol;
        if (config.serpentine) {
          y -= yinc;
          yinc = -yinc;
        } else {
          y = ystart;
        }
        x += xinc;
      }
    }
    if (values[i] > 0)
      drawList->AddRectFilled(ImVec2(x, y), ImVec2(x + size, y + size),
                              colors[values[i] - 1]);
    else if (values[i] < 0)
      drawList->AddRect(ImVec2(x, y), ImVec2(x + size, y + size),
                        colors[-values[i] - 1], 0.0f, 0, 1.0);
    if (sources) {
      ImGui::SetCursorScreenPos(ImVec2(x - sized2, y - sized2));
      if (sources[i]) {
        ImGui::PushID(i);
        ImGui::Selectable("", false, 0, ImVec2(inc, inc));
        sources[i]->EmitDrag();
        ImGui::PopID();
      } else {
        ImGui::Dummy(ImVec2(inc, inc));
      }
    }
    if (config.order == LEDConfig::RowMajor) {
      x += xinc;
    } else {
      y += yinc;
    }
  }

  if (!sources) ImGui::Dummy(ImVec2(inc * cols, inc * rows));
}

void DrawLEDs(const int* values, int numValues, int cols, const ImU32* colors,
              float size, float spacing, const LEDConfig& config) {
  DrawLEDSources(values, nullptr, numValues, cols, colors, size, spacing,
                 config);
}

}  // namespace halsimgui
