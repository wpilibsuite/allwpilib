// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

#include <ntcore_cpp.h>

#include "glass/DataSource.h"
#include "glass/networktables/NetworkTablesHelper.h"
#include "glass/other/Subsystem.h"

namespace glass {
class NTSubsystemModel : public SubsystemModel {
 public:
  static constexpr const char* kType = "Subsystem";

  explicit NTSubsystemModel(std::string_view path);
  NTSubsystemModel(NT_Inst instance, std::string_view path);

  const char* GetName() const override { return m_nameValue.c_str(); }
  const char* GetDefaultCommand() const override {
    return m_defaultCommandValue.c_str();
  }
  const char* GetCurrentCommand() const override {
    return m_currentCommandValue.c_str();
  }

  void Update() override;
  bool Exists() override;
  bool IsReadOnly() override { return true; }

 private:
  NetworkTablesHelper m_nt;
  NT_Entry m_name;
  NT_Entry m_defaultCommand;
  NT_Entry m_currentCommand;

  std::string m_nameValue;
  std::string m_defaultCommandValue;
  std::string m_currentCommandValue;
};
}  // namespace glass
