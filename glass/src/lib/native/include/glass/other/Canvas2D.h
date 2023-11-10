// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <span>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

#include "glass/Model.h"
#include "glass/View.h"

namespace glass {

class Canvas2DLineModel : public Model {
 public:
  virtual ImVec2 GetPoint1() const = 0;
  virtual ImVec2 GetPoint2() const = 0;
  virtual double GetWeight() const = 0;
  virtual ImU32 GetColor() const = 0;
  virtual int GetZOrder() const = 0;
};

class Canvas2DModel : public Model {
 public:
  virtual std::span<const Canvas2DLineModel*> GetLines() = 0;
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
}
