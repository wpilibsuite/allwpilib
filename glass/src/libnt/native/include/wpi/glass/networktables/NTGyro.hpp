// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

#include "wpi/nt/DoubleTopic.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/StringTopic.hpp"

#include "wpi/glass/DataSource.hpp"
#include "wpi/glass/hardware/Gyro.hpp"

namespace glass {
class NTGyroModel : public GyroModel {
 public:
  static constexpr const char* kType = "Gyro";

  explicit NTGyroModel(std::string_view path);
  NTGyroModel(nt::NetworkTableInstance inst, std::string_view path);

  const char* GetName() const override { return m_nameValue.c_str(); }
  const char* GetSimDevice() const override { return nullptr; }

  DoubleSource* GetAngleData() override { return &m_angleData; }
  void SetAngle(double value) override {}

  void Update() override;
  bool Exists() override;
  bool IsReadOnly() override { return true; }

 private:
  nt::NetworkTableInstance m_inst;
  nt::DoubleSubscriber m_angle;
  nt::StringSubscriber m_name;

  DoubleSource m_angleData;
  std::string m_nameValue;
};
}  // namespace glass
