// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/hardware/Gyro.h"

#include <cmath>

#define IMGUI_DEFINE_MATH_OPERATORS

#include <imgui.h>
#include <imgui_internal.h>
#include <numbers>

#include "glass/Context.h"
#include "glass/DataSource.h"

using namespace glass;

void glass::DisplayGyro(GyroModel* m) {
  ImColor primaryColor = ImGui::GetStyle().Colors[ImGuiCol_Text];
  ImColor disabledColor = ImGui::GetStyle().Colors[ImGuiCol_TextDisabled];
  ImColor secondaryColor = ImGui::GetStyle().Colors[ImGuiCol_Header];

  auto angle = m->GetAngleData();
  if (!angle || !m->Exists()) {
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(96, 96, 96, 255));
    ImGui::Text("Unknown Gyro");
    ImGui::PopStyleColor();
    return;
  }

  // Display the numeric angle value. This can be editable in some cases (i.e.
  // running from HALSIM).
  auto flags =
      m->IsReadOnly() ? ImGuiInputTextFlags_ReadOnly : ImGuiInputTextFlags_None;
  auto value = angle->GetValue();
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
  if (ImGui::InputDouble("Gyro Angle (Deg)", &value, 0.0, 0.0, "%.4f", flags)) {
    m->SetAngle(value);
  }

  // Draw the gyro indicator.
  ImDrawList* draw = ImGui::GetWindowDrawList();
  ImVec2 window = ImGui::GetWindowPos();
  float w = ImGui::GetWindowWidth();
  float h = ImGui::GetWindowHeight();

  float radius = (w < h) ? w * 0.3 : h * 0.3;
  ImVec2 center = window + ImVec2(w / 2, h / 2 + ImGui::GetFontSize());

  // Add the primary circle.
  draw->AddCircle(center, radius, primaryColor, 100, 1.5);

  // Draw the spokes at every 5 degrees and a "major" spoke every 45 degrees.
  for (int i = -175; i <= 180; i += 5) {
    double radians = i * 2 * std::numbers::pi / 360.0;
    ImVec2 direction(std::sin(radians), -std::cos(radians));

    bool major = i % 45 == 0;
    auto color = major ? primaryColor : disabledColor;

    draw->AddLine(center + (direction * radius),
                  center + (direction * radius * (major ? 1.07f : 1.03f)),
                  color, 1.2f);
    if (major) {
      char txt[16];
      std::snprintf(txt, sizeof(txt), "%d°", i);
      draw->AddText(
          center + (direction * radius * 1.25) - ImGui::CalcTextSize(txt) * 0.5,
          primaryColor, txt, nullptr);
    }
  }

  draw->AddCircleFilled(center, radius * 0.075, secondaryColor, 50);

  double radians = value * 2 * std::numbers::pi / 360.0;
  draw->AddLine(
      center - ImVec2(1, 0),
      center + ImVec2(std::sin(radians), -std::cos(radians)) * radius * 0.95f,
      secondaryColor, 3);
}
