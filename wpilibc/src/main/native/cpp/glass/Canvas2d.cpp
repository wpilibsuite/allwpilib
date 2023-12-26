// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/glass/Canvas2d.h"

#include <string_view>

#include <networktables/NTSendableBuilder.h>

using namespace frc;

static constexpr std::string_view kDims = "dims";
static constexpr std::string_view kLines = "lines";
static constexpr std::string_view kQuads = "quads";
static constexpr std::string_view kCircles = "circles";
static constexpr std::string_view kNgons = "ngons";
static constexpr std::string_view kTexts = "texts";

void Canvas2d::Clear() {
  m_currentZOrder = 0;
  m_lines.clear();
  m_quads.clear();
  m_circles.clear();
  m_ngons.clear();
  m_texts.clear();
}

void Canvas2d::DrawLine(float x1, float y1, float x2, float y2, float weight,
                        const frc::Color8Bit& color, int opacity) {
  m_lines.emplace_back(x1, y1, x2, y2, weight, color, opacity,
                       m_currentZOrder++);
}

void Canvas2d::DrawRect(float x1, float y1, float x2, float y2, float weight,
                        bool fill, const frc::Color8Bit& color, int opacity) {
  DrawQuad(x1, y1, x2, y1, x2, y2, x1, y2, weight, fill, color, opacity);
}

void Canvas2d::DrawQuad(float x1, float y1, float x2, float y2, float x3,
                        float y3, float x4, float y4, float weight, bool fill,
                        const frc::Color8Bit& color, int opacity) {
  m_quads.emplace_back(x1, y1, x2, y2, x3, y3, x4, y4, weight, fill, color,
                       opacity, m_currentZOrder++);
}

void Canvas2d::DrawCircle(float x, float y, float radius, float weight,
                          bool fill, const frc::Color8Bit& color, int opacity) {
  m_circles.emplace_back(x, y, radius, weight, fill, color, opacity,
                         m_currentZOrder++);
}

void Canvas2d::DrawNgon(float x, float y, float radius, int numSides,
                        float weight, bool fill, const frc::Color8Bit& color,
                        int opacity) {
  m_ngons.emplace_back(x, y, radius, numSides, weight, fill, color, opacity,
                       m_currentZOrder++);
}

void Canvas2d::DrawText(float x, float y, float fontSize,
                        const std::string_view& text,
                        const frc::Color8Bit& color, int opacity,
                        float wrapWidth) {
  m_texts.emplace_back(x, y, fontSize, text, color, opacity, wrapWidth,
                       m_currentZOrder++);
}

void Canvas2d::FinishFrame(bool clearCanvas) {
  if (m_linesPub) {
    m_linesPub.Set(m_lines);
  }
  if (m_quadsPub) {
    m_quadsPub.Set(m_quads);
  }
  if (m_circlesPub) {
    m_circlesPub.Set(m_circles);
  }
  if (m_ngonsPub) {
    m_ngonsPub.Set(m_ngons);
  }
  if (m_textsPub) {
    m_textsPub.Set(m_texts);
  }

  if (clearCanvas) {
    Clear();
  }
}

void Canvas2d::InitSendable(nt::NTSendableBuilder& builder) {
  builder.SetSmartDashboardType("Canvas2d");

  std::shared_ptr<nt::NetworkTable> table = builder.GetTable();
  m_dimsPub = table->GetFloatArrayTopic(kDims).Publish();
  m_dimsPub.Set({{m_width, m_height}});

  m_linesPub = table->GetStructArrayTopic<CanvasLine2d>(kLines).Publish();
  m_linesPub.Set({});

  m_quadsPub = table->GetStructArrayTopic<CanvasQuad2d>(kQuads).Publish();
  m_quadsPub.Set({});

  m_circlesPub = table->GetStructArrayTopic<CanvasCircle2d>(kCircles).Publish();
  m_circlesPub.Set({});

  m_ngonsPub = table->GetStructArrayTopic<CanvasNgon2d>(kNgons).Publish();
  m_ngonsPub.Set({});

  m_textsPub = table->GetStructArrayTopic<CanvasText2d>(kTexts).Publish();
  m_textsPub.Set({});
}
