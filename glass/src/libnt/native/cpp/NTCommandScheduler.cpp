// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NTCommandScheduler.h"

#include <fmt/format.h>
#include <wpi/StringExtras.h>

using namespace glass;

NTCommandSchedulerModel::NTCommandSchedulerModel(std::string_view path)
    : NTCommandSchedulerModel(nt::GetDefaultInstance(), path) {}

NTCommandSchedulerModel::NTCommandSchedulerModel(NT_Inst instance,
                                                 std::string_view path)
    : m_nt(instance),
      m_name(m_nt.GetEntry(fmt::format("{}/.name", path))),
      m_commands(m_nt.GetEntry(fmt::format("{}/Names", path))),
      m_ids(m_nt.GetEntry(fmt::format("{}/Ids", path))),
      m_cancel(m_nt.GetEntry(fmt::format("{}/Cancel", path))),
      m_nameValue(wpi::rsplit(path, '/').second) {
  m_nt.AddListener(m_name);
  m_nt.AddListener(m_commands);
  m_nt.AddListener(m_ids);
  m_nt.AddListener(m_cancel);
}

void NTCommandSchedulerModel::CancelCommand(size_t index) {
  if (index < m_idsValue.size()) {
    nt::SetEntryValue(
        m_cancel, nt::NetworkTableValue::MakeDoubleArray({m_idsValue[index]}));
  }
}

void NTCommandSchedulerModel::Update() {
  for (auto&& event : m_nt.PollListener()) {
    if (event.entry == m_name) {
      if (event.value && event.value->IsString()) {
        m_nameValue = event.value->GetString();
      }
    } else if (event.entry == m_commands) {
      if (event.value && event.value->IsStringArray()) {
        auto arr = event.value->GetStringArray();
        m_commandsValue.assign(arr.begin(), arr.end());
      }
    } else if (event.entry == m_ids) {
      if (event.value && event.value->IsDoubleArray()) {
        auto arr = event.value->GetDoubleArray();
        m_idsValue.assign(arr.begin(), arr.end());
      }
    }
  }
}

bool NTCommandSchedulerModel::Exists() {
  return m_nt.IsConnected() && nt::GetEntryType(m_commands) != NT_UNASSIGNED;
}
