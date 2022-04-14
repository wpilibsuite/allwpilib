// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

#include <ntcore_cpp.h>

#include "glass/DataSource.h"
#include "glass/hardware/Gyro.h"
#include "glass/networktables/NetworkTablesHelper.h"

namespace glass {
class NTGyroModel : public GyroModel {
 public:
  static constexpr const char* kType = "Gyro";

  explicit NTGyroModel(std::string_view path);
  NTGyroModel(NT_Inst instance, std::string_view path);

  const char* GetName() const override { return m_nameValue.c_str(); }
  const char* GetSimDevice() const override { return nullptr; }

  DataSource* GetAngleData() override { return &m_angleData; }
  void SetAngle(double value) override {}

  void Update() override;
  bool Exists() override;
  bool IsReadOnly() override { return true; }

 private:
  NetworkTablesHelper m_nt;
  NT_Entry m_angle;
  NT_Entry m_name;

  DataSource m_angleData;
  std::string m_nameValue;
};
}  // namespace glass
