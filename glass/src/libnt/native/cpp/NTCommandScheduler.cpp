// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NTCommandScheduler.h"

#include <utility>

#include <fmt/format.h>
#include <wpi/StringExtras.h>

using namespace glass;

NTCommandSchedulerModel::NTCommandSchedulerModel(std::string_view path)
    : NTCommandSchedulerModel(nt::NetworkTableInstance::GetDefault(), path) {}

NTCommandSchedulerModel::NTCommandSchedulerModel(nt::NetworkTableInstance inst,
                                                 std::string_view path)
    : m_inst{inst},
      m_name{inst.GetStringTopic(fmt::format("{}/.name", path)).Subscribe("")},
      m_commands{inst.GetStringArrayTopic(fmt::format("{}/Names", path))
                     .Subscribe({})},
      m_ids{
          inst.GetIntegerArrayTopic(fmt::format("{}/Ids", path)).Subscribe({})},
      m_cancel{
          inst.GetIntegerArrayTopic(fmt::format("{}/Cancel", path)).Publish()},
      m_nameValue{wpi::rsplit(path, '/').second} {}

void NTCommandSchedulerModel::CancelCommand(size_t index) {
  if (index < m_idsValue.size()) {
    m_cancel.Set({{m_idsValue[index]}});
  }
}

void NTCommandSchedulerModel::Update() {
  for (auto&& v : m_name.ReadQueue()) {
    m_nameValue = std::move(v.value);
  }
  for (auto&& v : m_commands.ReadQueue()) {
    m_commandsValue = std::move(v.value);
  }
  for (auto&& v : m_ids.ReadQueue()) {
    m_idsValue = std::move(v.value);
  }
}

bool NTCommandSchedulerModel::Exists() {
  return m_commands.Exists();
}
