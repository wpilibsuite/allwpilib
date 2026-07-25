// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/glass/networktables/NTCommandSelector.hpp"

#include <format>
#include <utility>

#include "wpi/util/StringExtras.hpp"

using namespace wpi::glass;

NTCommandSelectorModel::NTCommandSelectorModel(std::string_view path)
    : NTCommandSelectorModel(wpi::nt::NetworkTableInstance::GetDefault(),
                             path) {}

NTCommandSelectorModel::NTCommandSelectorModel(
    wpi::nt::NetworkTableInstance inst, std::string_view path)
    : m_inst{inst},
      m_running{inst.GetBooleanTopic(std::format("{}/running", path))
                    .GetEntry(false)},
      m_name{inst.GetStringTopic(std::format("{}/.name", path)).Subscribe("")},
      m_runningData{std::format("NTCmd:{}", path)},
      m_nameValue{wpi::util::rsplit(path, '/').second} {}

void NTCommandSelectorModel::SetRunning(bool run) {
  m_running.Set(run);
}

void NTCommandSelectorModel::Update() {
  for (auto&& v : m_running.ReadQueue()) {
    m_runningData.SetValue(v.value, v.time);
  }
  for (auto&& v : m_name.ReadQueue()) {
    m_nameValue = std::move(v.value);
  }
}

bool NTCommandSelectorModel::Exists() {
  return m_running.Exists();
}
