// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

#include <networktables/BooleanTopic.h>
#include <networktables/DoubleTopic.h>
#include <networktables/NetworkTableInstance.h>
#include <networktables/StringTopic.h>

#include "glass/DataSource.h"
#include "glass/hardware/SpeedController.h"

namespace glass {
class NTSpeedControllerModel : public SpeedControllerModel {
 public:
  static constexpr const char* kType = "Motor Controller";

  explicit NTSpeedControllerModel(std::string_view path);
  NTSpeedControllerModel(nt::NetworkTableInstance inst, std::string_view path);

  const char* GetName() const override { return m_nameValue.c_str(); }
  const char* GetSimDevice() const override { return nullptr; }

  DataSource* GetPercentData() override { return &m_valueData; }
  void SetPercent(double value) override;

  void Update() override;
  bool Exists() override;
  bool IsReadOnly() override { return !m_controllableValue; }

 private:
  nt::NetworkTableInstance m_inst;
  nt::DoubleEntry m_value;
  nt::StringSubscriber m_name;
  nt::BooleanSubscriber m_controllable;

  DataSource m_valueData;
  std::string m_nameValue;
  bool m_controllableValue = false;
};
}  // namespace glass
