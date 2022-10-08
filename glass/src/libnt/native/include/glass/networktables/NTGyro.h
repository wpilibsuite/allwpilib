// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

#include <networktables/DoubleTopic.h>
#include <networktables/NetworkTableInstance.h>
#include <networktables/StringTopic.h>

#include "glass/DataSource.h"
#include "glass/hardware/Gyro.h"

namespace glass {
class NTGyroModel : public GyroModel {
 public:
  static constexpr const char* kType = "Gyro";

  explicit NTGyroModel(std::string_view path);
  NTGyroModel(nt::NetworkTableInstance inst, std::string_view path);

  const char* GetName() const override { return m_nameValue.c_str(); }
  const char* GetSimDevice() const override { return nullptr; }

  DataSource* GetAngleData() override { return &m_angleData; }
  void SetAngle(double value) override {}

  void Update() override;
  bool Exists() override;
  bool IsReadOnly() override { return true; }

 private:
  nt::NetworkTableInstance m_inst;
  nt::DoubleSubscriber m_angle;
  nt::StringSubscriber m_name;

  DataSource m_angleData;
  std::string m_nameValue;
};
}  // namespace glass
