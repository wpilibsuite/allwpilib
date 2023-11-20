// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <vector>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

#include "glass/Model.h"
#include "glass/View.h"

namespace glass {

class Canvas2DLine {
 public:
  Canvas2DLine(ImVec2 point1, ImVec2 point2, float weight, ImU32 color,
               int zOrder)
      : m_point1{point1},
        m_point2{point2},
        m_weight{weight},
        m_color{color},
        m_zOrder{zOrder} {}

  ImVec2 GetPoint1() const { return m_point1; }
  ImVec2 GetPoint2() const { return m_point2; }
  float GetWeight() const { return m_weight; }
  ImU32 GetColor() const { return m_color; }
  int GetZOrder() const { return m_zOrder; }

 private:
  ImVec2 m_point1;
  ImVec2 m_point2;
  float m_weight;
  ImU32 m_color;
  int m_zOrder;
};

class Canvas2DModel : public Model {
 public:
  virtual std::vector<Canvas2DLine> GetLines() const = 0;
  virtual ImVec2 GetDimensions() const = 0;
};

void DisplayCanvas2D(Canvas2DModel* model, const ImVec2& contentSize);
void DisplayCanvas2DSettings(Canvas2DModel* model);

class Canvas2DView : public View {
 public:
  explicit Canvas2DView(Canvas2DModel* model) : m_model{model} {}

  void Display() override;
  void Settings() override;
  bool HasSettings() override;

 private:
  Canvas2DModel* m_model;
};
}  // namespace glass
