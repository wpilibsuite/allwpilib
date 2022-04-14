// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>
#include <vector>

#include <ntcore_cpp.h>

#include "glass/DataSource.h"
#include "glass/networktables/NetworkTablesHelper.h"
#include "glass/other/CommandScheduler.h"

namespace glass {
class NTCommandSchedulerModel : public CommandSchedulerModel {
 public:
  static constexpr const char* kType = "Scheduler";

  explicit NTCommandSchedulerModel(std::string_view path);
  NTCommandSchedulerModel(NT_Inst instance, std::string_view path);

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
