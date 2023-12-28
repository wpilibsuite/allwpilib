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
#include "glass/other/ProfiledPIDController.h"

namespace glass {
class NTProfiledPIDControllerModel : public ProfiledPIDControllerModel {
 public:
  static constexpr const char* kType = "ProfiledPIDController";

  explicit NTProfiledPIDControllerModel(std::string_view path);
  NTProfiledPIDControllerModel(nt::NetworkTableInstance inst,
                               std::string_view path);

  const char* GetName() const override { return m_nameValue.c_str(); }

  DataSource* GetPData() override { return &m_pData; }
  DataSource* GetIData() override { return &m_iData; }
  DataSource* GetDData() override { return &m_dData; }
  DataSource* GetGoalData() override { return &m_goalData; }
  DataSource* GetIZoneData() override { return &m_iZoneData; }

  void SetP(double value) override;
  void SetI(double value) override;
  void SetD(double value) override;
  void SetGoal(double value) override;
  void SetIZone(double value) override;

  void Update() override;
  bool Exists() override;
  bool IsReadOnly() override { return !m_controllableValue; }

 private:
  nt::NetworkTableInstance m_inst;
  nt::StringSubscriber m_name;
  nt::BooleanSubscriber m_controllable;
  nt::DoubleEntry m_p;
  nt::DoubleEntry m_i;
  nt::DoubleEntry m_d;
  nt::DoubleEntry m_goal;
  nt::DoubleEntry m_iZone;

  DataSource m_pData;
  DataSource m_iData;
  DataSource m_dData;
  DataSource m_goalData;
  DataSource m_iZoneData;

  std::string m_nameValue;
  bool m_controllableValue = false;
};
}  // namespace glass
