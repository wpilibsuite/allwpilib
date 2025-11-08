// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

#include "wpi/glass/DataSource.hpp"
#include "wpi/glass/other/CommandSelector.hpp"
#include "wpi/nt/BooleanTopic.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/StringTopic.hpp"

namespace wpi::glass {
class NTCommandSelectorModel : public CommandSelectorModel {
 public:
  static constexpr const char* kType = "Command";

  explicit NTCommandSelectorModel(std::string_view path);
  NTCommandSelectorModel(wpi::nt::NetworkTableInstance inst, std::string_view path);

  const char* GetName() const override { return m_nameValue.c_str(); }
  BooleanSource* GetRunningData() override { return &m_runningData; }
  void SetRunning(bool run) override;

  void Update() override;
  bool Exists() override;
  bool IsReadOnly() override { return false; }

 private:
  wpi::nt::NetworkTableInstance m_inst;
  wpi::nt::BooleanEntry m_running;
  wpi::nt::StringSubscriber m_name;

  BooleanSource m_runningData;
  std::string m_nameValue;
};
}  // namespace wpi::glass
