// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NTSubsystem.h"

using namespace glass;

NTSubsystemModel::NTSubsystemModel(wpi::StringRef path)
    : NTSubsystemModel(nt::GetDefaultInstance(), path) {}

NTSubsystemModel::NTSubsystemModel(NT_Inst instance, wpi::StringRef path)
    : m_nt(instance),
      m_name(m_nt.GetEntry(path + "/.name")),
      m_defaultCommand(m_nt.GetEntry(path + "/.default")),
      m_currentCommand(m_nt.GetEntry(path + "/.command")) {
  m_nt.AddListener(m_name);
  m_nt.AddListener(m_defaultCommand);
  m_nt.AddListener(m_currentCommand);
  Update();
}

void NTSubsystemModel::Update() {
  for (auto&& event : m_nt.PollListener()) {
    if (event.entry == m_name) {
      if (event.value && event.value->IsString()) {
        m_nameValue = event.value->GetString();
      }
    } else if (event.entry == m_defaultCommand) {
      if (event.value && event.value->IsString()) {
        m_defaultCommandValue = event.value->GetString();
      }
    } else if (event.entry == m_currentCommand) {
      if (event.value && event.value->IsString()) {
        m_currentCommandValue = event.value->GetString();
      }
    }
  }
}

bool NTSubsystemModel::Exists() {
  return m_nt.IsConnected() &&
         nt::GetEntryType(m_defaultCommand) != NT_UNASSIGNED;
}
