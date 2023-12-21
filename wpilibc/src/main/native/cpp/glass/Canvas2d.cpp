// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/glass/Canvas2d.h"

#include <string_view>

#include <networktables/NTSendableBuilder.h>

using namespace frc;

static constexpr std::string_view kDims = "dims";
static constexpr std::string_view kLines = "lines";

void Canvas2d::Clear() {
  m_currentZOrder = 0;
  m_lines.clear();
}

void Canvas2d::DrawLine(float x1, float y1, float x2, float y2, float weight,
                        const frc::Color8Bit& color, int opacity) {
  m_lines.emplace_back(x1, y1, x2, y2, weight, color, opacity,
                       m_currentZOrder++);
}

void Canvas2d::FinishFrame(bool clearCanvas) {
  if (m_linesPub) {
    m_linesPub.Set(m_lines);
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
}
