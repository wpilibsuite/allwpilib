// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NTSubsystem.h"

#include <utility>

#include <fmt/format.h>

using namespace glass;

NTSubsystemModel::NTSubsystemModel(std::string_view path)
    : NTSubsystemModel(nt::NetworkTableInstance::GetDefault(), path) {}

NTSubsystemModel::NTSubsystemModel(nt::NetworkTableInstance inst,
                                   std::string_view path)
    : m_inst{inst},
      m_name{inst.GetStringTopic(fmt::format("{}/.name", path)).Subscribe("")},
      m_defaultCommand{
          inst.GetStringTopic(fmt::format("{}/.default", path)).Subscribe("")},
      m_currentCommand{
          inst.GetStringTopic(fmt::format("{}/.command", path)).Subscribe("")} {
}

void NTSubsystemModel::Update() {
  for (auto&& v : m_name.ReadQueue()) {
    m_nameValue = std::move(v.value);
  }
  for (auto&& v : m_defaultCommand.ReadQueue()) {
    m_defaultCommandValue = std::move(v.value);
  }
  for (auto&& v : m_currentCommand.ReadQueue()) {
    m_currentCommandValue = std::move(v.value);
  }
}

bool NTSubsystemModel::Exists() {
  return m_defaultCommand.Exists();
}
