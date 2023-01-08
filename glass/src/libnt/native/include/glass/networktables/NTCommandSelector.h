// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

#include <networktables/BooleanTopic.h>
#include <networktables/NetworkTableInstance.h>
#include <networktables/StringTopic.h>

#include "glass/DataSource.h"
#include "glass/other/CommandSelector.h"

namespace glass {
class NTCommandSelectorModel : public CommandSelectorModel {
 public:
  static constexpr const char* kType = "Command";

  explicit NTCommandSelectorModel(std::string_view path);
  NTCommandSelectorModel(nt::NetworkTableInstance inst, std::string_view path);

  const char* GetName() const override { return m_nameValue.c_str(); }
  DataSource* GetRunningData() override { return &m_runningData; }
  void SetRunning(bool run) override;

  void Update() override;
  bool Exists() override;
  bool IsReadOnly() override { return false; }

 private:
  nt::NetworkTableInstance m_inst;
  nt::BooleanEntry m_running;
  nt::StringSubscriber m_name;

  DataSource m_runningData;
  std::string m_nameValue;
};
}  // namespace glass
