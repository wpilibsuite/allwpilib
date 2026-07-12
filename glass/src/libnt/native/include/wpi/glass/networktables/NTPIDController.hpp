// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

#include "wpi/glass/DataSource.hpp"
#include "wpi/glass/other/PIDController.hpp"
#include "wpi/nt/BooleanTopic.hpp"
#include "wpi/nt/DoubleTopic.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/StringTopic.hpp"

namespace wpi::glass {
class NTPIDControllerModel : public PIDControllerModel {
 public:
  static constexpr const char* kType = "PIDController";

  explicit NTPIDControllerModel(std::string_view path);
  NTPIDControllerModel(wpi::nt::NetworkTableInstance inst,
                       std::string_view path);

  const char* GetName() const override { return m_nameValue.c_str(); }

  DoubleSource* GetPData() override { return &m_pData; }
  DoubleSource* GetIData() override { return &m_iData; }
  DoubleSource* GetDData() override { return &m_dData; }
  DoubleSource* GetSetpointData() override { return &m_setpointData; }
  DoubleSource* GetIZoneData() override { return &m_iZoneData; }

  void SetP(double value) override;
  void SetI(double value) override;
  void SetD(double value) override;
  void SetSetpoint(double value) override;
  void SetIZone(double value) override;

  void Update() override;
  bool Exists() override;
  bool IsReadOnly() override { return !m_controllableValue; }

 private:
  wpi::nt::NetworkTableInstance m_inst;
  wpi::nt::StringSubscriber m_name;
  wpi::nt::BooleanSubscriber m_controllable;
  wpi::nt::DoubleEntry m_p;
  wpi::nt::DoubleEntry m_i;
  wpi::nt::DoubleEntry m_d;
  wpi::nt::DoubleEntry m_setpoint;
  wpi::nt::DoubleEntry m_iZone;

  DoubleSource m_pData;
  DoubleSource m_iData;
  DoubleSource m_dData;
  DoubleSource m_setpointData;
  DoubleSource m_iZoneData;

  std::string m_nameValue;
  bool m_controllableValue = false;
};
}  // namespace wpi::glass
