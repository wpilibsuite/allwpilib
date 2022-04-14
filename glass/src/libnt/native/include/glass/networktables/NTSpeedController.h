// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

#include <ntcore_cpp.h>

#include "glass/DataSource.h"
#include "glass/hardware/SpeedController.h"
#include "glass/networktables/NetworkTablesHelper.h"

namespace glass {
class NTSpeedControllerModel : public SpeedControllerModel {
 public:
  static constexpr const char* kType = "Motor Controller";

  explicit NTSpeedControllerModel(std::string_view path);
  NTSpeedControllerModel(NT_Inst instance, std::string_view path);

  const char* GetName() const override { return m_nameValue.c_str(); }
  const char* GetSimDevice() const override { return nullptr; }

  DataSource* GetPercentData() override { return &m_valueData; }
  void SetPercent(double value) override;

  void Update() override;
  bool Exists() override;
  bool IsReadOnly() override { return !m_controllableValue; }

 private:
  NetworkTablesHelper m_nt;
  NT_Entry m_value;
  NT_Entry m_name;
  NT_Entry m_controllable;

  DataSource m_valueData;
  std::string m_nameValue;
  bool m_controllableValue = false;
};
}  // namespace glass
