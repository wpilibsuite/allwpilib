// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/other/Drive.h"

#include <array>
#include <cmath>

#define IMGUI_DEFINE_MATH_OPERATORS

#include <imgui.h>
#include <imgui_internal.h>
#include <numbers>

#include "glass/Context.h"
#include "glass/DataSource.h"

using namespace glass;

void glass::DisplayDrive(DriveModel* m) {
  // Check if the model exists.
  if (!m->Exists()) {
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(96, 96, 96, 255));
    ImGui::Text("Unknown Drive");
    ImGui::PopStyleColor();
    return;
  }

  const auto& wheels = m->GetWheels();
  ImDrawList* draw = ImGui::GetWindowDrawList();
  ImColor color = ImGui::GetStyle().Colors[ImGuiCol_Text];

  // Get window position and size.
  ImVec2 pos = ImGui::GetWindowPos();
  ImVec2 size = ImGui::GetWindowSize();

  // Calculate corners for drivetrain body.
  float x1 = pos.x + 60.0f;
  float y1 = pos.y + ImGui::GetFontSize() * 2.0f;
  float x2 = pos.x + size.x - 60.0f;
  float y2 = pos.y + size.y - ImGui::GetFontSize() * 2.0f * wheels.size();

  // Draw the primary rectangle.
  draw->AddRect(ImVec2(x1, y1), ImVec2(x2, y2), color);

  // Display the speed vector.
  ImVec2 center{(x1 + x2) / 2.0f, (y1 + y2) / 2.0f};
  ImVec2 speed = m->GetSpeedVector();
  ImVec2 arrow = center + speed * 50.0f;

  draw->AddLine(center, arrow, color, 2.0f);

  auto drawArrow = [draw, &color](const ImVec2& arrowPos, float angle) {
    draw->AddTriangleFilled(
        arrowPos,
        arrowPos + ImRotate(ImVec2(0.0f, 7.5f),
                            std::cos(angle + std::numbers::pi / 4),
                            std::sin(angle + std::numbers::pi / 4)),
        arrowPos + ImRotate(ImVec2(0.0f, 7.5f),
                            std::cos(angle - std::numbers::pi / 4),
                            std::sin(angle - std::numbers::pi / 4)),
        color);
  };

  // Draw the arrow if there is any translation; draw an X otherwise.
  if (std::abs(speed.y) > 0 || std::abs(speed.x) > 0) {
    drawArrow(arrow, std::atan2(speed.x, -speed.y));
  } else {
    ImVec2 a{7.5f, +7.5f};
    ImVec2 b{7.5f, -7.5f};
    draw->AddLine(center + a, center - a, color);
    draw->AddLine(center + b, center - b, color);
  }

  // Calculate the positions of the top-left corner of the wheels.
  std::array<ImVec2, 4> corners{
      ImVec2(x1 - 25.0f, y1 + 10.0f), ImVec2(x1 - 25.0f, y2 - 70.0f),
      ImVec2(x2 + 00.0f, y1 + 10.0f), ImVec2(x2 + 00.0f, y2 - 70.0f)};

  // Draw the wheels.
  for (auto&& corner : corners) {
    draw->AddRect(corner, corner + ImVec2(25.0f, 60.0f), color);
  }

  // Show rotation
  double rotation = m->GetRotation();
  if (rotation != 0) {
    float radius = 60.0f;
    double a1 = 0.0;
    double a2 = std::numbers::pi / 2 * rotation;

    // PathArcTo requires a_min <= a_max, and rotation can be negative
    if (a1 > a2) {
      draw->PathArcTo(center, radius, a2, a1, 20);
      draw->PathStroke(color, false);
      draw->PathArcTo(center, radius, a2 + std::numbers::pi,
                      a1 + std::numbers::pi, 20);
      draw->PathStroke(color, false);
    } else {
      draw->PathArcTo(center, radius, a1, a2, 20);
      draw->PathStroke(color, false);
      draw->PathArcTo(center, radius, a1 + std::numbers::pi,
                      a2 + std::numbers::pi, 20);
      draw->PathStroke(color, false);
    }

    double adder = rotation < 0 ? std::numbers::pi : 0;

    auto arrowPos =
        center + ImVec2(radius * -std::cos(a2), radius * -std::sin(a2));
    drawArrow(arrowPos, a2 + adder);

    a2 += std::numbers::pi;
    arrowPos = center + ImVec2(radius * -std::cos(a2), radius * -std::sin(a2));
    drawArrow(arrowPos, a2 + adder);
  }

  // Set the buttons and sliders to read-only if the model is read-only.
  if (m->IsReadOnly()) {
    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(210, 210, 210, 255));
  }

  // Add sliders for the wheel percentages.
  ImGui::SetCursorPosY(y2 - pos.y + ImGui::GetFontSize() * 0.5);
  for (auto&& wheel : wheels) {
    if (wheel.percent) {
      ImGui::PushID(wheel.name.c_str());
      if (ImGui::Button("Zero")) {
        wheel.setter(0.0);
      }
      ImGui::PopID();

      ImGui::SameLine();
      ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8.0f);
      float value = wheel.percent->GetValue();
      if (wheel.percent->SliderFloat(wheel.name.c_str(), &value, -1.0f, 1.0f)) {
        wheel.setter(value);
      }
    }
  }

  if (m->IsReadOnly()) {
    ImGui::PopStyleColor();
    ImGui::PopItemFlag();
  }
}
