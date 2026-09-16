// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include "wpi/glass/DataSource.hpp"
#include "wpi/glass/hardware/DIO.hpp"
#include "wpi/nt/BooleanTopic.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"

namespace wpi::glass {

class NTDigitalOutputModel : public DIOModel {
 public:
  static constexpr const char* TYPE = "Digital Output";

  // path is to the table containing ".type", excluding the trailing /
  explicit NTDigitalOutputModel(std::string_view path);
  NTDigitalOutputModel(wpi::nt::NetworkTableInstance inst,
                       std::string_view path);

  const char* GetSimDevice() const override { return nullptr; }

  DPWMModel* GetDPWM() override { return nullptr; }
  DutyCycleModel* GetDutyCycle() override { return nullptr; }
  EncoderModel* GetEncoder() override { return nullptr; }

  bool IsInput() const override { return false; }

  BooleanSource* GetValueData() override { return &m_valueData; }

  void SetValue(bool val) override;

  void Update() override;
  bool Exists() override;
  bool IsReadOnly() override;

 private:
  wpi::nt::NetworkTableInstance m_inst;
  wpi::nt::BooleanEntry m_value;

  BooleanSource m_valueData;
};

}  // namespace wpi::glass
