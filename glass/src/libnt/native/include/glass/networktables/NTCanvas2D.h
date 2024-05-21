// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <set>
#include <string>
#include <vector>

#include <imgui.h>
#include <networktables/DoubleArrayTopic.h>
#include <networktables/FloatArrayTopic.h>
#include <networktables/MultiSubscriber.h>
#include <networktables/NetworkTableInstance.h>
#include <networktables/NetworkTableListener.h>
#include <networktables/RawTopic.h>
#include <networktables/StringTopic.h>
#include <ntcore_cpp.h>
#include <wpi/struct/DynamicStruct.h>

#include "glass/other/Canvas2D.h"

namespace glass {

class NTCanvas2DModel : public Canvas2DModel {
 public:
  static constexpr const char* kType = "Canvas2d";

  explicit NTCanvas2DModel(std::string_view path,
                           wpi::StructDescriptorDatabase& structDatabase);
  NTCanvas2DModel(nt::NetworkTableInstance inst,
                  wpi::StructDescriptorDatabase& structDatabase,
                  std::string_view path);
  ~NTCanvas2DModel() override;

  const char* GetPath() const { return m_path.c_str(); }
  const char* GetName() const { return m_name.c_str(); }

  void Update() override;
  bool Exists() override;
  bool IsReadOnly() override;

  std::set<const Canvas2DElement*, Canvas2DElementSort> GetElements()
      const override;
  ImVec2 GetDimensions() const override;

 private:
  std::string m_path;
  nt::NetworkTableInstance m_inst;
  wpi::StructDescriptorDatabase& m_structDatabase;

  nt::StringSubscriber m_nameSub;
  nt::FloatArraySubscriber m_dimensionsSub;
  nt::RawSubscriber m_linesSub;
  nt::RawSubscriber m_quadsSub;
  nt::RawSubscriber m_circlesSub;
  nt::RawSubscriber m_ngonsSub;
  nt::RawSubscriber m_textsSub;

  std::string m_name;
  ImVec2 m_dimensions;

  std::set<const Canvas2DElement*, Canvas2DElementSort> m_elements;
  std::vector<Canvas2DLine> m_lines;
  std::vector<Canvas2DQuad> m_quads;
  std::vector<Canvas2DCircle> m_circles;
  std::vector<Canvas2DNgon> m_ngons;
  std::vector<Canvas2DText> m_texts;
};
}  // namespace glass
