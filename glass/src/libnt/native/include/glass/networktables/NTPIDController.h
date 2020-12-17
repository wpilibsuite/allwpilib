/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <string>

#include <ntcore_cpp.h>
#include <wpi/StringRef.h>

#include "glass/DataSource.h"
#include "glass/networktables/NetworkTablesHelper.h"
#include "glass/other/PIDController.h"

namespace glass {
class NTPIDControllerModel : public PIDControllerModel {
 public:
  static constexpr const char* kType = "PIDController";

  explicit NTPIDControllerModel(wpi::StringRef path);
  NTPIDControllerModel(NT_Inst instance, wpi::StringRef path);

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
  bool IsReadOnly() override { return false; }

 private:
  NetworkTablesHelper m_nt;
  NT_Entry m_name;
  NT_Entry m_p;
  NT_Entry m_i;
  NT_Entry m_d;
  NT_Entry m_setpoint;

  DataSource m_pData;
  DataSource m_iData;
  DataSource m_dData;
  DataSource m_setpointData;

  std::string m_nameValue;
};
}  // namespace glass
