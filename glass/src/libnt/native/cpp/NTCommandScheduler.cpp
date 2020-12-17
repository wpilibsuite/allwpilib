/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "glass/networktables/NTCommandScheduler.h"

#include <iostream>

using namespace glass;

NTCommandSchedulerModel::NTCommandSchedulerModel(wpi::StringRef path)
    : NTCommandSchedulerModel(nt::GetDefaultInstance(), path) {}

NTCommandSchedulerModel::NTCommandSchedulerModel(NT_Inst instance,
                                                 wpi::StringRef path)
    : m_nt(instance),
      m_name(m_nt.GetEntry(path + "/.name")),
      m_commands(m_nt.GetEntry(path + "/Names")),
      m_ids(m_nt.GetEntry(path + "/Ids")),
      m_cancel(m_nt.GetEntry(path + "/Cancel")),
      m_nameValue(path.rsplit('/').second) {
  m_nt.AddListener(m_name);
  m_nt.AddListener(m_commands);
  m_nt.AddListener(m_ids);
  m_nt.AddListener(m_cancel);
  Update();
}

void NTCommandSchedulerModel::CancelCommand(size_t index) {
  if (index < m_idsValue.size())
    nt::SetEntryValue(
        m_cancel, nt::NetworkTableValue::MakeDoubleArray({m_idsValue[index]}));
}

void NTCommandSchedulerModel::Update() {
  for (auto&& event : m_nt.PollListener()) {
    if (event.entry == m_name) {
      if (event.value && event.value->IsString())
        m_nameValue = event.value->GetString();
    } else if (event.entry == m_commands) {
      if (event.value && event.value->IsStringArray())
        m_commandsValue = event.value->GetStringArray();
    } else if (event.entry == m_ids) {
      if (event.value && event.value->IsDoubleArray())
        m_idsValue = event.value->GetDoubleArray();
    }
  }
}

bool NTCommandSchedulerModel::Exists() {
  return m_nt.IsConnected() && nt::GetEntryType(m_commands) != NT_UNASSIGNED;
}
