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
#include "glass/other/CommandSelector.h"

namespace glass {
class NTCommandSelectorModel : public CommandSelectorModel {
 public:
  static constexpr const char* kType = "Command";

  explicit NTCommandSelectorModel(wpi::StringRef path);
  NTCommandSelectorModel(NT_Inst instance, wpi::StringRef path);

  const char* GetName() const override { return m_nameValue.c_str(); }
  DataSource* GetRunningData() override { return &m_runningData; }
  void SetRunning(bool run) override;

  void Update() override;
  bool Exists() override;
  bool IsReadOnly() override { return false; }

 private:
  NetworkTablesHelper m_nt;
  NT_Entry m_running;
  NT_Entry m_name;

  DataSource m_runningData;
  std::string m_nameValue;
};
}  // namespace glass
