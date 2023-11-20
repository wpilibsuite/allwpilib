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
      m_dimensionsSub(
          inst.GetFloatArrayTopic(fmt::format("{}/dims", path)).Subscribe({})),
      m_linesSub(inst.GetRawTopic(fmt::format("{}/lines", path))
                     .Subscribe("struct:CanvasLine2d[]", {})) {}

NTCanvas2DModel::~NTCanvas2DModel() = default;

void NTCanvas2DModel::Update() {
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
    auto* zOrderField = lineDesc->FindFieldByName("zOrder");
    auto* redField = lineDesc->FindFieldByName("r");
    auto* greenField = lineDesc->FindFieldByName("g");
    auto* blueField = lineDesc->FindFieldByName("b");

    for (auto&& v : m_linesSub.ReadQueue()) {
      m_lines.clear();

      std::span<const unsigned char> raw(v.value);
      int amount = raw.size() / lineDesc->GetSize();
      for (int i = 0; i < amount; i++) {
        wpi::DynamicStruct asStruct{lineDesc, raw};

        ImVec2 p1{asStruct.GetFloatField(x1Field),
                  asStruct.GetFloatField(y1Field)};
        ImVec2 p2{asStruct.GetFloatField(x2Field),
                  asStruct.GetFloatField(y2Field)};
        float weight = asStruct.GetFloatField(weightField);
        ImU32 color = IM_COL32(asStruct.GetUintField(redField),
                               asStruct.GetUintField(greenField),
                               asStruct.GetUintField(blueField), 255);
        int zOrder = asStruct.GetIntField(zOrderField);

        m_lines.emplace_back(p1, p2, weight, color, zOrder);

        raw = wpi::drop_front(raw, lineDesc->GetSize());
      }
    }
  }
}

bool NTCanvas2DModel::Exists() {
  return m_dimensionsSub.Exists();
}

bool NTCanvas2DModel::IsReadOnly() {
  return true;
}

ImVec2 NTCanvas2DModel::GetDimensions() const {
  return m_dimensions;
}

std::vector<Canvas2DLine> NTCanvas2DModel::GetLines() const {
  return m_lines;
}
