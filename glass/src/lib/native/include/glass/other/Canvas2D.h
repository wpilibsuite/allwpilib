// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <set>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

#include "glass/Model.h"
#include "glass/View.h"

namespace glass {

class Canvas2DElement {
 public:
  virtual ~Canvas2DElement() = default;
  virtual void Draw(float scale, const ImVec2& cursorPos,
                    ImDrawList* drawList) const = 0;
  virtual int GetZOrder() const = 0;
};

struct Canvas2DElementSort {
  bool operator()(const Canvas2DElement* a, const Canvas2DElement* b) const {
    return a->GetZOrder() < b->GetZOrder();
  }
};

class Canvas2DLine : public Canvas2DElement {
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
  int GetZOrder() const override { return m_zOrder; }

  void Draw(float scale, const ImVec2& cursorPos,
            ImDrawList* drawList) const override;

 private:
  ImVec2 m_point1;
  ImVec2 m_point2;
  float m_weight;
  ImU32 m_color;
  int m_zOrder;
};

class Canvas2DQuad : public Canvas2DElement {
 public:
  Canvas2DQuad(ImVec2 point1, ImVec2 point2, ImVec2 point3, ImVec2 point4,
               float weight, bool fill, ImU32 color, int zOrder)
      : m_point1{point1},
        m_point2{point2},
        m_point3{point3},
        m_point4{point4},
        m_weight(weight),
        m_fill{fill},
        m_color{color},
        m_zOrder{zOrder} {}

  ImVec2 GetPoint1() const { return m_point1; }
  ImVec2 GetPoint2() const { return m_point2; }
  ImVec2 GetPoint3() const { return m_point3; }
  ImVec2 GetPoint4() const { return m_point4; }
  float GetWeight() const { return m_weight; }
  ImU32 GetColor() const { return m_color; }
  bool IsFill() const { return m_fill; }
  int GetZOrder() const override { return m_zOrder; }

  void Draw(float scale, const ImVec2& cursorPos,
            ImDrawList* drawList) const override;

 private:
  ImVec2 m_point1;
  ImVec2 m_point2;
  ImVec2 m_point3;
  ImVec2 m_point4;
  float m_weight;
  bool m_fill;
  ImU32 m_color;
  int m_zOrder;
};

class Canvas2DCircle : public Canvas2DElement {
 public:
  Canvas2DCircle(ImVec2 center, float radius, float weight, bool fill,
                 ImU32 color, int zOrder)
      : m_center{center},
        m_radius{radius},
        m_weight(weight),
        m_fill{fill},
        m_color{color},
        m_zOrder{zOrder} {}

  ImVec2 GetCenter() const { return m_center; }
  float GetRadius() const { return m_radius; }
  float GetWeight() const { return m_weight; }
  bool IsFill() const { return m_fill; }
  ImU32 GetColor() const { return m_color; }
  int GetZOrder() const override { return m_zOrder; }

  void Draw(float scale, const ImVec2& cursorPos,
            ImDrawList* drawList) const override;

 private:
  ImVec2 m_center;
  float m_radius;
  float m_weight;
  bool m_fill;
  ImU32 m_color;
  int m_zOrder;
};

class Canvas2DNgon : public Canvas2DElement {
 public:
  Canvas2DNgon(ImVec2 center, float radius, int numSides, float weight,
               bool fill, ImU32 color, int zOrder)
      : m_center{center},
        m_radius{radius},
        m_numSides{numSides},
        m_weight{weight},
        m_fill{fill},
        m_color{color},
        m_zOrder{zOrder} {}

  ImVec2 GetCenter() const { return m_center; }
  float GetRadius() const { return m_radius; }
  int GetNumSides() const { return m_numSides; }
  float GetWeight() const { return m_weight; }
  bool IsFill() const { return m_fill; }
  ImU32 GetColor() const { return m_color; }
  int GetZOrder() const override { return m_zOrder; }

  void Draw(float scale, const ImVec2& cursorPos,
            ImDrawList* drawList) const override;

 private:
  ImVec2 m_center;
  float m_radius;
  int m_numSides;
  float m_weight;
  bool m_fill;
  ImU32 m_color;
  int m_zOrder;
};

class Canvas2DText : public Canvas2DElement {
 public:
  Canvas2DText(ImVec2 position, float fontSize, std::string text, ImU32 color,
               float wrapWidth, int zOrder)
      : m_position{position},
        m_fontSize{fontSize},
        m_text(std::move(text)),
        m_color{color},
        m_wrapWidth{wrapWidth},
        m_zOrder{zOrder} {}

  ImVec2 GetPosition() const { return m_position; }
  float GetFontSize() const { return m_fontSize; }
  std::string_view GetText() const { return m_text; }
  ImU32 GetColor() const { return m_color; }
  float GetWrapWidth() const { return m_wrapWidth; }
  int GetZOrder() const override { return m_zOrder; }

  void Draw(float scale, const ImVec2& cursorPos,
            ImDrawList* drawList) const override;

 private:
  ImVec2 m_position;
  float m_fontSize;
  std::string m_text;
  ImU32 m_color;
  float m_wrapWidth;
  int m_zOrder;
};

class Canvas2DModel : public Model {
 public:
  virtual std::set<const Canvas2DElement*, Canvas2DElementSort> GetElements()
      const = 0;
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
