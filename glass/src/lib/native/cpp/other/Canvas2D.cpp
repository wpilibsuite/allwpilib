// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/other/Canvas2D.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <wpigui.h>

using namespace glass;
namespace gui = wpi::gui;

void glass::DisplayCanvas2D(Canvas2DModel *model, const ImVec2 &contentSize) {
  if (contentSize.x <= 0 || contentSize.y <= 0) {
    return;
  }

  ImGui::InvisibleButton("background", contentSize);
  auto drawList = ImGui::GetWindowDrawList();

  auto lines = model->GetLines();

  // TODO: make this traverse multuple lists
  for (auto line : lines) {
    drawList->AddLine(line->GetPoint1(), line->GetPoint2(), line->GetColor(),
                      line->GetWeight());
  }
}
