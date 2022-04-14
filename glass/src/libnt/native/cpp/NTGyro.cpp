// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NTGyro.h"

#include <fmt/format.h>
#include <wpi/StringExtras.h>

using namespace glass;

NTGyroModel::NTGyroModel(std::string_view path)
    : NTGyroModel(nt::GetDefaultInstance(), path) {}

NTGyroModel::NTGyroModel(NT_Inst instance, std::string_view path)
    : m_nt(instance),
      m_angle(m_nt.GetEntry(fmt::format("{}/Value", path))),
      m_name(m_nt.GetEntry(fmt::format("{}/.name", path))),
      m_angleData(fmt::format("NT_Gyro:{}", path)),
      m_nameValue(wpi::rsplit(path, '/').second) {
  m_nt.AddListener(m_angle);
  m_nt.AddListener(m_name);
}

void NTGyroModel::Update() {
  for (auto&& event : m_nt.PollListener()) {
    if (event.entry == m_angle) {
      if (event.value && event.value->IsDouble()) {
        m_angleData.SetValue(event.value->GetDouble());
      }
    } else if (event.entry == m_name) {
      if (event.value && event.value->IsString()) {
        m_nameValue = event.value->GetString();
      }
    }
  }
}

bool NTGyroModel::Exists() {
  return m_nt.IsConnected() && nt::GetEntryType(m_angle) != NT_UNASSIGNED;
}
