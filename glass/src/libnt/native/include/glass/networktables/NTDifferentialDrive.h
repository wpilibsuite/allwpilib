// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>
#include <vector>

#include <networktables/BooleanTopic.h>
#include <networktables/DoubleTopic.h>
#include <networktables/NetworkTableInstance.h>
#include <networktables/StringTopic.h>

#include "glass/DataSource.h"
#include "glass/other/Drive.h"

namespace glass {
class NTDifferentialDriveModel : public DriveModel {
 public:
  static constexpr const char* kType = "DifferentialDrive";

  explicit NTDifferentialDriveModel(std::string_view path);
  NTDifferentialDriveModel(nt::NetworkTableInstance instance,
                           std::string_view path);

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
  nt::NetworkTableInstance m_inst;
  nt::StringSubscriber m_name;
  nt::BooleanSubscriber m_controllable;
  nt::DoubleEntry m_lPercent;
  nt::DoubleEntry m_rPercent;

  std::string m_nameValue;
  bool m_controllableValue = false;
  DoubleSource m_lPercentData;
  DoubleSource m_rPercentData;

  std::vector<DriveModel::WheelInfo> m_wheels;
  ImVec2 m_speedVector;
  double m_rotation;
};
}  // namespace glass
