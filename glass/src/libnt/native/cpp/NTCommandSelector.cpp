/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "glass/networktables/NTCommandSelector.h"

using namespace glass;

NTCommandSelectorModel::NTCommandSelectorModel(wpi::StringRef path)
    : NTCommandSelectorModel(nt::GetDefaultInstance(), path) {}

NTCommandSelectorModel::NTCommandSelectorModel(NT_Inst instance,
                                               wpi::StringRef path)
    : m_nt(instance),
      m_running(m_nt.GetEntry(path + "/running")),
      m_name(m_nt.GetEntry(path + "/.name")),
      m_runningData("NTCmd:" + path),
      m_nameValue(path.rsplit('/').second) {
  m_runningData.SetDigital(true);
  m_nt.AddListener(m_running);
  m_nt.AddListener(m_name);
  Update();
}

void NTCommandSelectorModel::SetRunning(bool run) {
  nt::SetEntryValue(m_running, nt::NetworkTableValue::MakeBoolean(run));
}

void NTCommandSelectorModel::Update() {
  for (auto&& event : m_nt.PollListener()) {
    if (event.entry == m_running) {
      if (event.value && event.value->IsBoolean())
        m_runningData.SetValue(event.value->GetBoolean());
    } else if (event.entry == m_name) {
      if (event.value && event.value->IsString())
        m_nameValue = event.value->GetString();
    }
  }
}

bool NTCommandSelectorModel::Exists() {
  return m_nt.IsConnected() && nt::GetEntryType(m_running) != NT_UNASSIGNED;
}
