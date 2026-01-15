// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "wpi/glass/DataSource.hpp"
#include "wpi/glass/other/Drive.hpp"
#include "wpi/nt/BooleanTopic.hpp"
#include "wpi/nt/DoubleTopic.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/StringTopic.hpp"

namespace wpi::glass {
class NTMecanumDriveModel : public DriveModel {
 public:
  static constexpr const char* kType = "MecanumDrive";

  explicit NTMecanumDriveModel(std::string_view path);
  NTMecanumDriveModel(wpi::nt::NetworkTableInstance inst,
                      std::string_view path);

  const char* GetName() const override { return m_nameValue.c_str(); }
  const std::vector<DriveModel::WheelInfo>& GetWheels() const override {
    return m_wheels;
  }

  ImVec2 GetVelocityVector() const override { return m_velocityVector; }
  double GetRotation() const override { return m_rotation; }

  void Update() override;
  bool Exists() override;
  bool IsReadOnly() override { return !m_controllableValue; }

 private:
  wpi::nt::NetworkTableInstance m_inst;
  wpi::nt::StringSubscriber m_name;
  wpi::nt::BooleanSubscriber m_controllable;
  wpi::nt::DoubleEntry m_flPercent;
  wpi::nt::DoubleEntry m_frPercent;
  wpi::nt::DoubleEntry m_rlPercent;
  wpi::nt::DoubleEntry m_rrPercent;

  std::string m_nameValue;
  bool m_controllableValue = false;
  DoubleSource m_flPercentData;
  DoubleSource m_frPercentData;
  DoubleSource m_rlPercentData;
  DoubleSource m_rrPercentData;

  std::vector<DriveModel::WheelInfo> m_wheels;
  ImVec2 m_velocityVector;
  double m_rotation;
};
}  // namespace wpi::glass
