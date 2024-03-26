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

#include "frc/glass/CanvasCircle2d.h"
#include "frc/glass/CanvasLine2d.h"
#include "frc/glass/CanvasNgon2d.h"
#include "frc/glass/CanvasQuad2d.h"
#include "frc/glass/CanvasText2d.h"
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

  void DrawText(float x, float y, float fontSize, const std::string_view& text,
                const frc::Color8Bit& color, int opacity = 255,
                float wrapWidth = 0);

  void FinishFrame(bool clearCanvas = true);

  void InitSendable(nt::NTSendableBuilder& builder) override;

 private:
  int m_currentZOrder = 0;

  const float m_width;
  const float m_height;
  std::vector<frc::CanvasLine2d> m_lines;
  std::vector<frc::CanvasQuad2d> m_quads;
  std::vector<frc::CanvasCircle2d> m_circles;
  std::vector<frc::CanvasNgon2d> m_ngons;
  std::vector<frc::CanvasText2d> m_texts;

  nt::FloatArrayPublisher m_dimsPub;
  nt::StructArrayPublisher<frc::CanvasLine2d> m_linesPub;
  nt::StructArrayPublisher<frc::CanvasQuad2d> m_quadsPub;
  nt::StructArrayPublisher<frc::CanvasCircle2d> m_circlesPub;
  nt::StructArrayPublisher<frc::CanvasNgon2d> m_ngonsPub;
  nt::StructArrayPublisher<frc::CanvasText2d> m_textsPub;
};

}  // namespace frc
