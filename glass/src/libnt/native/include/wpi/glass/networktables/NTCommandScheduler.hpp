// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <string>
#include <string_view>
#include <vector>

#include "wpi/glass/DataSource.hpp"
#include "wpi/glass/other/CommandScheduler.hpp"
#include "wpi/nt/IntegerArrayTopic.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/StringArrayTopic.hpp"
#include "wpi/nt/StringTopic.hpp"

namespace wpi::glass {
class NTCommandSchedulerModel : public CommandSchedulerModel {
 public:
  static constexpr const char* kType = "Scheduler";

  explicit NTCommandSchedulerModel(std::string_view path);
  NTCommandSchedulerModel(wpi::nt::NetworkTableInstance inst,
                          std::string_view path);

  const char* GetName() const override { return m_nameValue.c_str(); }
  const std::vector<std::string>& GetCurrentCommands() override {
    return m_commandsValue;
  }

  void CancelCommand(size_t index) override;

  void Update() override;
  bool Exists() override;
  bool IsReadOnly() override { return false; }

 private:
  wpi::nt::NetworkTableInstance m_inst;
  wpi::nt::StringSubscriber m_name;
  wpi::nt::StringArraySubscriber m_commands;
  wpi::nt::IntegerArraySubscriber m_ids;
  wpi::nt::IntegerArrayPublisher m_cancel;

  std::string m_nameValue;
  std::vector<std::string> m_commandsValue;
  std::vector<int64_t> m_idsValue;
};
}  // namespace wpi::glass
