// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/other/Canvas2D.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <wpigui.h>

using namespace glass;
namespace gui = wpi::gui;

void glass::DisplayCanvas2D(Canvas2DModel* model, const ImVec2& contentSize) {
  if (contentSize.x <= 0 || contentSize.y <= 0) {
    return;
  }

  ImVec2 dimensions = model->GetDimensions();

  float scale;
  ImVec2 cursorPos = ImGui::GetWindowPos() + ImGui::GetCursorPos();
  if (contentSize.x / dimensions.x > contentSize.y / dimensions.y) {
    scale = contentSize.y / dimensions.y;
    cursorPos.x += (contentSize.x - dimensions.x * scale) / 2;
  } else {
    scale = contentSize.x / dimensions.x;
    cursorPos.y += (contentSize.y - dimensions.y * scale) / 2;
  }

  auto drawList = ImGui::GetWindowDrawList();

  auto lines = model->GetLines();

  // TODO: make this traverse multuple lists
  for (auto line : lines) {
    drawList->AddLine(line.GetPoint1() * scale + cursorPos,
                      line.GetPoint2() * scale + cursorPos, line.GetColor(),
                      line.GetWeight() * scale);
  }
}

void glass::DisplayCanvas2DSettings(Canvas2DModel* model) {
  // TODO
}

void Canvas2DView::Display() {
  DisplayCanvas2D(m_model, ImGui::GetWindowContentRegionMax() -
                               ImGui::GetWindowContentRegionMin());
  ;
}

void Canvas2DView::Settings() {
  DisplayCanvas2DSettings(m_model);
}

bool Canvas2DView::HasSettings() {
  // TODO
  return false;
}
