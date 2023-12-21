// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <vector>

#include <networktables/FloatArrayTopic.h>
#include <networktables/NTSendable.h>
#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableEntry.h>
#include <networktables/StructArrayTopic.h>
#include <wpi/sendable/SendableHelper.h>

#include "CanvasLine2d.h"
#include "frc/util/Color8Bit.h"

namespace frc {

class Canvas2d : public nt::NTSendable, public wpi::SendableHelper<Canvas2d> {
 public:
  Canvas2d(float width, float height) : m_width{width}, m_height{height} {}

  void Clear();

  void DrawLine(float x1, float y1, float x2, float y2, float weight,
                const frc::Color8Bit& color, int opacity = 255);

  void DrawRect(float x1, float y1, float x2, float y2, float weight, bool fill,
                const frc::Color8Bit& color, int opacity = 255);

  void DrawQuad(float x1, float y1, float x2, float y2, float x3, float y3,
                float x4, float y4, float weight, bool fill,
                const frc::Color8Bit& color, int opacity = 255);

  void DrawCircle(float x, float y, float radius, float weight, bool fill,
                  const frc::Color8Bit& color, int opacity = 255);

  void DrawNgon(float x, float y, float radius, int numSides, float weight,
                bool fill, const frc::Color8Bit& color, int opacity = 255);

  void FinishFrame(bool clearCanvas = true);

  void InitSendable(nt::NTSendableBuilder& builder) override;

 private:
  int m_currentZOrder = 0;

  const float m_width;
  const float m_height;
  std::vector<frc::CanvasLine2d> m_lines;

  nt::FloatArrayPublisher m_dimsPub;
  nt::StructArrayPublisher<frc::CanvasLine2d> m_linesPub;
};

}  // namespace frc
