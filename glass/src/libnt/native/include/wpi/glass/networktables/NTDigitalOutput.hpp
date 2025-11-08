// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

#include "wpi/glass/DataSource.hpp"
#include "wpi/glass/hardware/DIO.hpp"
#include "wpi/nt/BooleanTopic.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/StringTopic.hpp"

namespace wpi::glass {

class NTDigitalOutputModel : public DIOModel {
 public:
  static constexpr const char* kType = "Digital Output";

  // path is to the table containing ".type", excluding the trailing /
  explicit NTDigitalOutputModel(std::string_view path);
  NTDigitalOutputModel(wpi::nt::NetworkTableInstance inst, std::string_view path);

  const char* GetName() const override { return m_nameValue.c_str(); }

  const char* GetSimDevice() const override { return nullptr; }

  DPWMModel* GetDPWM() override { return nullptr; }
  DutyCycleModel* GetDutyCycle() override { return nullptr; }
  EncoderModel* GetEncoder() override { return nullptr; }

  bool IsInput() const override { return true; }

  BooleanSource* GetValueData() override { return &m_valueData; }

  void SetValue(bool val) override;

  void Update() override;
  bool Exists() override;
  bool IsReadOnly() override { return !m_controllableValue; }

 private:
  wpi::nt::NetworkTableInstance m_inst;
  wpi::nt::BooleanEntry m_value;
  wpi::nt::StringSubscriber m_name;
  wpi::nt::BooleanSubscriber m_controllable;

  BooleanSource m_valueData;
  std::string m_nameValue;
  bool m_controllableValue = false;
};

}  // namespace wpi::glass
