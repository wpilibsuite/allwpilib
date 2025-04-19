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
#include "glass/other/ExponentialPIDController.h"

namespace glass {
class NTExponentialPIDControllerModel : public ExponentialPIDControllerModel {
 public:
  static constexpr const char* kType = "ExponentialPIDController";

  explicit NTExponentialPIDControllerModel(std::string_view path);
  NTExponentialPIDControllerModel(nt::NetworkTableInstance inst,
                                  std::string_view path);

  const char* GetName() const override { return m_nameValue.c_str(); }

  DataSource* GetPData() override { return &m_pData; }
  DataSource* GetIData() override { return &m_iData; }
  DataSource* GetDData() override { return &m_dData; }
  DataSource* GetIZoneData() override { return &m_iZoneData; }
  DataSource* GetMaxInputData() override { return &m_maxInputData; }
  DataSource* GetAData() override { return &m_aData; }
  DataSource* GetBData() override { return &m_bData; }
  DataSource* GetGoalData() override { return &m_goalData; }

  void SetP(double value) override;
  void SetI(double value) override;
  void SetD(double value) override;
  void SetIZone(double value) override;
  void SetV(double value) override;
  void SetA(double value) override;
  void SetGoal(double value) override;

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
  nt::DoubleEntry m_iZone;
  nt::DoubleEntry m_maxInput;
  nt::DoubleEntry m_a;
  nt::DoubleEntry m_b;
  nt::DoubleEntry m_goal;

  DataSource m_pData;
  DataSource m_iData;
  DataSource m_dData;
  DataSource m_iZoneData;
  DataSource m_maxInputData;
  DataSource m_aData;
  DataSource m_bData;
  DataSource m_goalData;

  std::string m_nameValue;
  bool m_controllableValue = false;
};
}  // namespace glass
