// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

#include <ntcore_cpp.h>

#include "glass/DataSource.h"
#include "glass/networktables/NetworkTablesHelper.h"
#include "glass/other/PIDController.h"

namespace glass {
class NTPIDControllerModel : public PIDControllerModel {
 public:
  static constexpr const char* kType = "PIDController";

  explicit NTPIDControllerModel(std::string_view path);
  NTPIDControllerModel(NT_Inst instance, std::string_view path);

  const char* GetName() const override { return m_nameValue.c_str(); }

  DataSource* GetPData() override { return &m_pData; }
  DataSource* GetIData() override { return &m_iData; }
  DataSource* GetDData() override { return &m_dData; }
  DataSource* GetSetpointData() override { return &m_setpointData; }

  void SetP(double value) override;
  void SetI(double value) override;
  void SetD(double value) override;
  void SetSetpoint(double value) override;

  void Update() override;
  bool Exists() override;
  bool IsReadOnly() override { return !m_controllableValue; }

 private:
  NetworkTablesHelper m_nt;
  NT_Entry m_name;
  NT_Entry m_controllable;
  NT_Entry m_p;
  NT_Entry m_i;
  NT_Entry m_d;
  NT_Entry m_setpoint;

  DataSource m_pData;
  DataSource m_iData;
  DataSource m_dData;
  DataSource m_setpointData;

  std::string m_nameValue;
  bool m_controllableValue = false;
};
}  // namespace glass
