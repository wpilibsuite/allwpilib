// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NTCanvas2D.h"

#include <memory>
#include <span>

#include <fmt/format.h>
#include <imgui.h>
#include <networktables/NetworkTableInstance.h>
#include <ntcore_cpp.h>
#include <wpi/SpanExtras.h>
#include <wpi/struct/DynamicStruct.h>

#include "glass/other/Canvas2D.h"

using namespace glass;

NTCanvas2DModel::NTCanvas2DModel(std::string_view path,
                                 wpi::StructDescriptorDatabase& structDatabase)
    : NTCanvas2DModel(nt::NetworkTableInstance::GetDefault(), structDatabase,
                      path) {}

// TODO: Default value for dims
NTCanvas2DModel::NTCanvas2DModel(nt::NetworkTableInstance inst,
                                 wpi::StructDescriptorDatabase& structDatabase,
                                 std::string_view path)
    : m_path{fmt::format("{}/", path)},
      m_inst{inst},
      m_structDatabase{structDatabase},
      m_nameSub(
          inst.GetStringTopic(fmt::format("{}/.name", path)).Subscribe("")),
      m_dimensionsSub(
          inst.GetFloatArrayTopic(fmt::format("{}/dims", path)).Subscribe({})),
      m_linesSub(inst.GetRawTopic(fmt::format("{}/lines", path))
                     .Subscribe("struct:CanvasLine2d[]", {})),
      m_quadsSub(inst.GetRawTopic(fmt::format("{}/quads", path))
                     .Subscribe("struct:CanvasQuad2d[]", {})),
      m_circlesSub(inst.GetRawTopic(fmt::format("{}/circles", path))
                       .Subscribe("struct:CanvasCircle2d[]", {})),
      m_ngonsSub(inst.GetRawTopic(fmt::format("{}/ngons", path))
                     .Subscribe("struct:CanvasNgon2d[]", {})),
      m_textsSub(inst.GetRawTopic(fmt::format("{}/texts", path))
                     .Subscribe("struct:CanvasText2d[]", {})) {}

NTCanvas2DModel::~NTCanvas2DModel() = default;

void NTCanvas2DModel::Update() {
  for (auto&& v : m_nameSub.ReadQueue()) {
    m_name = v.value;
  }

  for (auto&& v : m_dimensionsSub.ReadQueue()) {
    if (v.value.size() == 2) {
      m_dimensions.x = v.value[0];
      m_dimensions.y = v.value[1];
    } else {
      m_dimensions.x = 0;
      m_dimensions.y = 0;
    }
  }

  const wpi::StructDescriptor* lineDesc = m_structDatabase.Find("CanvasLine2d");
  if (lineDesc) {
    auto* x1Field = lineDesc->FindFieldByName("x1");
    auto* y1Field = lineDesc->FindFieldByName("y1");
    auto* x2Field = lineDesc->FindFieldByName("x2");
    auto* y2Field = lineDesc->FindFieldByName("y2");
    auto* weightField = lineDesc->FindFieldByName("weight");
    auto* colorField = lineDesc->FindFieldByName("color");
    auto* zOrderField = lineDesc->FindFieldByName("zOrder");

    for (auto&& v : m_linesSub.ReadQueue()) {
      m_lines.clear();

      wpi::DynamicStructArray lineArray{lineDesc, v.value};
      for (const auto& line : lineArray) {
        ImVec2 point1{line.GetFloatField(x1Field), line.GetFloatField(y1Field)};
        ImVec2 point2{line.GetFloatField(x2Field), line.GetFloatField(y2Field)};
        float weight = line.GetFloatField(weightField);
        ImU32 color = IM_COL32(
            line.GetUintField(colorField, 0), line.GetUintField(colorField, 1),
            line.GetUintField(colorField, 2), line.GetUintField(colorField, 3));
        int zOrder = line.GetIntField(zOrderField);

        m_lines.emplace_back(point1, point2, weight, color, zOrder);
      }
    }
  }

  const wpi::StructDescriptor* quadDesc = m_structDatabase.Find("CanvasQuad2d");
  if (quadDesc) {
    auto* x1Field = quadDesc->FindFieldByName("x1");
    auto* y1Field = quadDesc->FindFieldByName("y1");
    auto* x2Field = quadDesc->FindFieldByName("x2");
    auto* y2Field = quadDesc->FindFieldByName("y2");
    auto* x3Field = quadDesc->FindFieldByName("x3");
    auto* y3Field = quadDesc->FindFieldByName("y3");
    auto* x4Field = quadDesc->FindFieldByName("x4");
    auto* y4Field = quadDesc->FindFieldByName("y4");
    auto* weightField = quadDesc->FindFieldByName("weight");
    auto* fillField = quadDesc->FindFieldByName("fill");
    auto* colorField = quadDesc->FindFieldByName("color");
    auto* zOrderField = quadDesc->FindFieldByName("zOrder");

    for (auto&& v : m_quadsSub.ReadQueue()) {
      m_quads.clear();

      wpi::DynamicStructArray quadArray{quadDesc, v.value};
      for (const auto& quad : quadArray) {
        ImVec2 point1{quad.GetFloatField(x1Field), quad.GetFloatField(y1Field)};
        ImVec2 point2{quad.GetFloatField(x2Field), quad.GetFloatField(y2Field)};
        ImVec2 point3{quad.GetFloatField(x3Field), quad.GetFloatField(y3Field)};
        ImVec2 point4{quad.GetFloatField(x4Field), quad.GetFloatField(y4Field)};
        float weight = quad.GetFloatField(weightField);
        ImU32 color = IM_COL32(
            quad.GetUintField(colorField, 0), quad.GetUintField(colorField, 1),
            quad.GetUintField(colorField, 2), quad.GetUintField(colorField, 3));
        bool fill = quad.GetBoolField(fillField);
        int zOrder = quad.GetIntField(zOrderField);

        m_quads.emplace_back(point1, point2, point3, point4, weight, fill,
                             color, zOrder);
      }
    }
  }

  const wpi::StructDescriptor* circleDesc =
      m_structDatabase.Find("CanvasCircle2d");
  if (circleDesc) {
    auto* xField = circleDesc->FindFieldByName("x");
    auto* yField = circleDesc->FindFieldByName("y");
    auto* radiusField = circleDesc->FindFieldByName("radius");
    auto* weightField = circleDesc->FindFieldByName("weight");
    auto* fillField = circleDesc->FindFieldByName("fill");
    auto* colorField = circleDesc->FindFieldByName("color");
    auto* zOrderField = circleDesc->FindFieldByName("zOrder");

    for (auto&& v : m_circlesSub.ReadQueue()) {
      m_circles.clear();

      wpi::DynamicStructArray circleArray{circleDesc, v.value};
      for (const auto& circle : circleArray) {
        ImVec2 center{circle.GetFloatField(xField),
                      circle.GetFloatField(yField)};
        float radius = circle.GetFloatField(radiusField);
        float weight = circle.GetFloatField(weightField);
        bool fill = circle.GetBoolField(fillField);
        ImU32 color = IM_COL32(circle.GetUintField(colorField, 0),
                               circle.GetUintField(colorField, 1),
                               circle.GetUintField(colorField, 2),
                               circle.GetUintField(colorField, 3));
        int zOrder = circle.GetIntField(zOrderField);

        m_circles.emplace_back(center, radius, weight, fill, color, zOrder);
      }
    }
  }

  const wpi::StructDescriptor* ngonDesc = m_structDatabase.Find("CanvasNgon2d");
  if (ngonDesc) {
    auto* xField = ngonDesc->FindFieldByName("x");
    auto* yField = ngonDesc->FindFieldByName("y");
    auto* radiusField = ngonDesc->FindFieldByName("radius");
    auto* numSidesField = ngonDesc->FindFieldByName("numSides");
    auto* weightField = ngonDesc->FindFieldByName("weight");
    auto* fillField = ngonDesc->FindFieldByName("fill");
    auto* colorField = ngonDesc->FindFieldByName("color");
    auto* zOrderField = ngonDesc->FindFieldByName("zOrder");

    for (auto&& v : m_ngonsSub.ReadQueue()) {
      m_ngons.clear();

      wpi::DynamicStructArray ngonArray{ngonDesc, v.value};
      for (const auto& ngon : ngonArray) {
        ImVec2 center{ngon.GetFloatField(xField), ngon.GetFloatField(yField)};
        float radius = ngon.GetFloatField(radiusField);
        int numSides = ngon.GetIntField(numSidesField);
        float weight = ngon.GetFloatField(weightField);
        bool fill = ngon.GetBoolField(fillField);
        ImU32 color = IM_COL32(
            ngon.GetUintField(colorField, 0), ngon.GetUintField(colorField, 1),
            ngon.GetUintField(colorField, 2), ngon.GetUintField(colorField, 3));
        int zOrder = ngon.GetIntField(zOrderField);

        m_ngons.emplace_back(center, radius, numSides, weight, fill, color,
                             zOrder);
      }
    }
  }

  const wpi::StructDescriptor* textDesc = m_structDatabase.Find("CanvasText2d");
  if (textDesc) {
    auto* xField = textDesc->FindFieldByName("x");
    auto* yField = textDesc->FindFieldByName("y");
    auto* fontSizeField = textDesc->FindFieldByName("fontSize");
    auto* textField = textDesc->FindFieldByName("text");
    auto* colorField = textDesc->FindFieldByName("color");
    auto* wrapWidthField = textDesc->FindFieldByName("wrapWidth");
    auto* zOrderField = textDesc->FindFieldByName("zOrder");

    for (auto&& v : m_textsSub.ReadQueue()) {
      m_texts.clear();

      wpi::DynamicStructArray textArray{textDesc, v.value};
      for (const auto& text : textArray) {
        ImVec2 position{text.GetFloatField(xField), text.GetFloatField(yField)};
        float fontSize = text.GetFloatField(fontSizeField);
        std::string textStr{text.GetStringField(textField)};
        ImU32 color = IM_COL32(
            text.GetUintField(colorField, 0), text.GetUintField(colorField, 1),
            text.GetUintField(colorField, 2), text.GetUintField(colorField, 3));
        float wrapWidth = text.GetFloatField(wrapWidthField);
        int zOrder = text.GetIntField(zOrderField);

        m_texts.emplace_back(position, fontSize, std::move(textStr), color,
                             wrapWidth, zOrder);
      }
    }
  }

  m_elements.clear();
  for (const auto& line : m_lines) {
    m_elements.insert(&line);
  }
  for (const auto& quad : m_quads) {
    m_elements.insert(&quad);
  }
  for (const auto& circle : m_circles) {
    m_elements.insert(&circle);
  }
  for (const auto& ngon : m_ngons) {
    m_elements.insert(&ngon);
  }
  for (const auto& text : m_texts) {
    m_elements.insert(&text);
  }
}

bool NTCanvas2DModel::Exists() {
  return m_nameSub.Exists();
}

bool NTCanvas2DModel::IsReadOnly() {
  return true;
}

ImVec2 NTCanvas2DModel::GetDimensions() const {
  return m_dimensions;
}

std::set<const Canvas2DElement*, Canvas2DElementSort>
NTCanvas2DModel::GetElements() const {
  return m_elements;
}
