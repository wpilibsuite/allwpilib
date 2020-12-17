/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <string>
#include <vector>

#include <ntcore_cpp.h>
#include <wpi/StringRef.h>

#include "glass/DataSource.h"
#include "glass/networktables/NetworkTablesHelper.h"
#include "glass/other/CommandScheduler.h"

namespace glass {
class NTCommandSchedulerModel : public CommandSchedulerModel {
 public:
  static constexpr const char* kType = "Scheduler";

  explicit NTCommandSchedulerModel(wpi::StringRef path);
  NTCommandSchedulerModel(NT_Inst instance, wpi::StringRef path);

  const char* GetName() const override { return m_nameValue.c_str(); }
  const std::vector<std::string>& GetCurrentCommands() override {
    return m_commandsValue;
  }

  void CancelCommand(size_t index) override;

  void Update() override;
  bool Exists() override;
  bool IsReadOnly() override { return false; }

 private:
  NetworkTablesHelper m_nt;
  NT_Entry m_name;
  NT_Entry m_commands;
  NT_Entry m_ids;
  NT_Entry m_cancel;

  std::string m_nameValue;
  std::vector<std::string> m_commandsValue;
  std::vector<double> m_idsValue;
};
}  // namespace glass
