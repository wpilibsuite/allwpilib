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
#include "glass/hardware/SpeedController.h"
#include "glass/networktables/NetworkTablesHelper.h"

namespace glass {
class NTSpeedControllerModel : public SpeedControllerModel {
 public:
  static constexpr const char* kType = "Speed Controller";

  explicit NTSpeedControllerModel(wpi::StringRef path);
  NTSpeedControllerModel(NT_Inst instance, wpi::StringRef path);

  const char* GetName() const override { return m_nameValue.c_str(); }
  const char* GetSimDevice() const override { return nullptr; }

  DataSource* GetPercentData() override { return &m_valueData; }
  void SetPercent(double value) override;

  void Update() override;
  bool Exists() override;
  bool IsReadOnly() override { return false; }

 private:
  NetworkTablesHelper m_nt;
  NT_Entry m_value;
  NT_Entry m_name;

  DataSource m_valueData;
  std::string m_nameValue;
};
}  // namespace glass
