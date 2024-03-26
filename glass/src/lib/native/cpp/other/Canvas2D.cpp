// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/other/Canvas2D.h"

#include <span>
#include <vector>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <wpigui.h>

using namespace glass;
namespace gui = wpi::gui;

void Canvas2DLine::Draw(float scale, const ImVec2& cursorPos,
                        ImDrawList* drawList) const {
  drawList->AddLine(cursorPos + (m_point1 * scale),
                    cursorPos + (m_point2 * scale), m_color, m_weight * scale);
}

void Canvas2DQuad::Draw(float scale, const ImVec2& cursorPos,
                        ImDrawList* drawList) const {
  if (m_fill) {
    drawList->AddQuadFilled(cursorPos + (m_point1 * scale),
                            cursorPos + (m_point2 * scale),
                            cursorPos + (m_point3 * scale),
                            cursorPos + (m_point4 * scale), m_color);
  } else {
    drawList->AddQuad(
        cursorPos + (m_point1 * scale), cursorPos + (m_point2 * scale),
        cursorPos + (m_point3 * scale), cursorPos + (m_point4 * scale), m_color,
        m_weight * scale);
  }
}

void Canvas2DCircle::Draw(float scale, const ImVec2& cursorPos,
                          ImDrawList* drawList) const {
  if (m_fill) {
    drawList->AddCircleFilled(cursorPos + (m_center * scale), m_radius * scale,
                              m_color);
  } else {
    drawList->AddCircle(cursorPos + (m_center * scale), m_radius * scale,
                        m_color, 0, m_weight * scale);
  }
}

void Canvas2DNgon::Draw(float scale, const ImVec2& cursorPos,
                        ImDrawList* drawList) const {
  if (m_fill) {
    drawList->AddNgonFilled(cursorPos + (m_center * scale), m_radius * scale,
                            m_color, m_numSides);
  } else {
    drawList->AddNgon(cursorPos + (m_center * scale), m_radius * scale, m_color,
                      m_numSides, m_weight * scale);
  }
}

void Canvas2DText::Draw(float scale, const ImVec2& cursorPos,
                        ImDrawList* drawList) const {
  drawList->AddText(nullptr, m_fontSize * scale,
                    cursorPos + (m_position * scale), m_color, m_text.c_str(),
                    nullptr, m_wrapWidth * scale);
}

void glass::DisplayCanvas2D(Canvas2DModel* model, const ImVec2& contentSize) {
  if (contentSize.x <= 0 || contentSize.y <= 0) {
    return;
  }

  ImVec2 dimensions = model->GetDimensions();
  ImDrawList* drawList = ImGui::GetWindowDrawList();

  float scale;
  ImVec2 cursorPos = ImGui::GetWindowPos() + ImGui::GetCursorPos();
  if (contentSize.x / dimensions.x > contentSize.y / dimensions.y) {
    scale = contentSize.y / dimensions.y;
    cursorPos.x += (contentSize.x - dimensions.x * scale) / 2;
  } else {
    scale = contentSize.x / dimensions.x;
    cursorPos.y += (contentSize.y - dimensions.y * scale) / 2;
  }

  for (auto&& element : model->GetElements()) {
    element->Draw(scale, cursorPos, drawList);
  }
}

void glass::DisplayCanvas2DSettings(Canvas2DModel* model) {
  // TODO
}

void Canvas2DView::Display() {
  DisplayCanvas2D(m_model, ImGui::GetWindowContentRegionMax() -
                               ImGui::GetWindowContentRegionMin());
}

void Canvas2DView::Settings() {
  DisplayCanvas2DSettings(m_model);
}

bool Canvas2DView::HasSettings() {
  // TODO
  return false;
}
