// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

#include "wpi/glass/DataSource.hpp"
#include "wpi/glass/hardware/MotorController.hpp"
#include "wpi/nt/BooleanTopic.hpp"
#include "wpi/nt/DoubleTopic.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/StringTopic.hpp"

namespace wpi::glass {
class NTMotorControllerModel : public MotorControllerModel {
 public:
  static constexpr const char* kType = "Motor Controller";

  explicit NTMotorControllerModel(std::string_view path);
  NTMotorControllerModel(wpi::nt::NetworkTableInstance inst, std::string_view path);

  const char* GetName() const override { return m_nameValue.c_str(); }
  const char* GetSimDevice() const override { return nullptr; }

  DoubleSource* GetPercentData() override { return &m_valueData; }
  void SetPercent(double value) override;

  void Update() override;
  bool Exists() override;
  bool IsReadOnly() override { return !m_controllableValue; }

 private:
  wpi::nt::NetworkTableInstance m_inst;
  wpi::nt::DoubleEntry m_value;
  wpi::nt::StringSubscriber m_name;
  wpi::nt::BooleanSubscriber m_controllable;

  DoubleSource m_valueData;
  std::string m_nameValue;
  bool m_controllableValue = false;
};
}  // namespace wpi::glass
