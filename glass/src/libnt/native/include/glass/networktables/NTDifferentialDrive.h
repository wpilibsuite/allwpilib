// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <vector>

#include <ntcore_cpp.h>
#include <wpi/STLExtras.h>
#include <wpi/StringRef.h>

#include "glass/DataSource.h"
#include "glass/networktables/NetworkTablesHelper.h"
#include "glass/other/Drive.h"

namespace glass {
class NTDifferentialDriveModel : public DriveModel {
 public:
  static constexpr const char* kType = "DifferentialDrive";

  explicit NTDifferentialDriveModel(wpi::StringRef path);
  NTDifferentialDriveModel(NT_Inst instance, wpi::StringRef path);

  const char* GetName() const override { return m_nameValue.c_str(); }
  const std::vector<DriveModel::WheelInfo>& GetWheels() const override {
    return m_wheels;
  }

  ImVec2 GetSpeedVector() const override { return m_speedVector; }
  double GetRotation() const override { return m_rotation; }

  void Update() override;
  bool Exists() override;
  bool IsReadOnly() override { return !m_controllableValue; }

 private:
  NetworkTablesHelper m_nt;
  NT_Entry m_name;
  NT_Entry m_controllable;
  NT_Entry m_lPercent;
  NT_Entry m_rPercent;

  std::string m_nameValue;
  bool m_controllableValue = false;
  DataSource m_lPercentData;
  DataSource m_rPercentData;

  std::vector<DriveModel::WheelInfo> m_wheels;
  ImVec2 m_speedVector;
  double m_rotation;
};
}  // namespace glass
