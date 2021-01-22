// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NTSpeedController.h"

using namespace glass;

NTSpeedControllerModel::NTSpeedControllerModel(wpi::StringRef path)
    : NTSpeedControllerModel(nt::GetDefaultInstance(), path) {}

NTSpeedControllerModel::NTSpeedControllerModel(NT_Inst instance,
                                               wpi::StringRef path)
    : m_nt(instance),
      m_value(m_nt.GetEntry(path + "/Value")),
      m_name(m_nt.GetEntry(path + "/.name")),
      m_controllable(m_nt.GetEntry(path + "/.controllable")),
      m_valueData("NT_SpdCtrl:" + path),
      m_nameValue(path.rsplit('/').second) {
  m_nt.AddListener(m_value);
  m_nt.AddListener(m_name);
  m_nt.AddListener(m_controllable);
}

void NTSpeedControllerModel::SetPercent(double value) {
  nt::SetEntryValue(m_value, nt::NetworkTableValue::MakeDouble(value));
}

void NTSpeedControllerModel::Update() {
  for (auto&& event : m_nt.PollListener()) {
    if (event.entry == m_value) {
      if (event.value && event.value->IsDouble()) {
        m_valueData.SetValue(event.value->GetDouble());
      }
    } else if (event.entry == m_name) {
      if (event.value && event.value->IsString()) {
        m_nameValue = event.value->GetString();
      }
    } else if (event.entry == m_controllable) {
      if (event.value && event.value->IsBoolean()) {
        m_controllableValue = event.value->GetBoolean();
      }
    }
  }
}

bool NTSpeedControllerModel::Exists() {
  return m_nt.IsConnected() && nt::GetEntryType(m_value) != NT_UNASSIGNED;
}
