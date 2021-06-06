// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <frc/geometry/Translation2d.h>
#include <ntcore_cpp.h>

#include "glass/networktables/NetworkTablesHelper.h"
#include "glass/other/Mechanism2D.h"

namespace glass {

class NTMechanism2DModel : public Mechanism2DModel {
 public:
  static constexpr const char* kType = "Mechanism2d";

  // path is to the table containing ".type", excluding the trailing /
  explicit NTMechanism2DModel(std::string_view path);
  NTMechanism2DModel(NT_Inst inst, std::string_view path);
  ~NTMechanism2DModel() override;

  const char* GetPath() const { return m_path.c_str(); }
  const char* GetName() const { return m_nameValue.c_str(); }

  void Update() override;
  bool Exists() override;
  bool IsReadOnly() override;

  frc::Translation2d GetDimensions() const override {
    return m_dimensionsValue;
  }
  ImU32 GetBackgroundColor() const override { return m_bgColorValue; }
  void ForEachRoot(
      wpi::function_ref<void(MechanismRootModel& model)> func) override;

 private:
  NetworkTablesHelper m_nt;
  std::string m_path;

  NT_Entry m_name;
  NT_Entry m_dimensions;
  NT_Entry m_bgColor;

  std::string m_nameValue;
  frc::Translation2d m_dimensionsValue;
  ImU32 m_bgColorValue = 0;

  class RootModel;
  std::vector<std::unique_ptr<RootModel>> m_roots;
};

}  // namespace glass
