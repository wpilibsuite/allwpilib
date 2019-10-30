/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ExtraGuiWidgets.h"

namespace halsimgui {

void DrawLEDs(int* values, int numValues, int cols, const ImU32* colors,
              float size, float spacing) {
  if (numValues == 0) return;
  if (size == 0) size = ImGui::GetFontSize() / 2.0;
  if (spacing == 0) spacing = ImGui::GetFontSize() / 3.0;

  ImDrawList* drawList = ImGui::GetWindowDrawList();
  const ImVec2 p = ImGui::GetCursorScreenPos();
  float x = p.x + size / 2, y = p.y + size / 2;
  int rows = 1;
  for (int i = 0; i < numValues; ++i) {
    if (i >= (rows * cols)) {
      ++rows;
      x = p.x + size / 2;
      y += size + spacing;
    }
    if (values[i] > 0)
      drawList->AddRectFilled(ImVec2(x, y), ImVec2(x + size, y + size),
                              colors[values[i] - 1]);
    else if (values[i] < 0)
      drawList->AddRect(ImVec2(x, y), ImVec2(x + size, y + size),
                        colors[-values[i] - 1], 0.0f, 0, 1.0);
    x += size + spacing;
  }
  ImGui::Dummy(ImVec2((size + spacing) * cols, (size + spacing) * rows));
}

}  // namespace halsimgui
