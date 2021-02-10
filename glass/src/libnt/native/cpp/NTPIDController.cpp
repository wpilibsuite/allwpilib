// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NTPIDController.h"

using namespace glass;

NTPIDControllerModel::NTPIDControllerModel(wpi::StringRef path)
    : NTPIDControllerModel(nt::GetDefaultInstance(), path) {}

NTPIDControllerModel::NTPIDControllerModel(NT_Inst instance,
                                           wpi::StringRef path)
    : m_nt(instance),
      m_name(m_nt.GetEntry(path + "/.name")),
      m_controllable(m_nt.GetEntry(path + "/.controllable")),
      m_p(m_nt.GetEntry(path + "/p")),
      m_i(m_nt.GetEntry(path + "/i")),
      m_d(m_nt.GetEntry(path + "/d")),
      m_setpoint(m_nt.GetEntry(path + "/setpoint")),
      m_pData("NTPIDCtrlP:" + path),
      m_iData("NTPIDCtrlI:" + path),
      m_dData("NTPIDCtrlD:" + path),
      m_setpointData("NTPIDCtrlStpt:" + path),
      m_nameValue(path.rsplit('/').second) {
  m_nt.AddListener(m_name);
  m_nt.AddListener(m_controllable);
  m_nt.AddListener(m_p);
  m_nt.AddListener(m_i);
  m_nt.AddListener(m_d);
  m_nt.AddListener(m_setpoint);
}

void NTPIDControllerModel::SetP(double value) {
  nt::SetEntryValue(m_p, nt::NetworkTableValue::MakeDouble(value));
}

void NTPIDControllerModel::SetI(double value) {
  nt::SetEntryValue(m_i, nt::NetworkTableValue::MakeDouble(value));
}

void NTPIDControllerModel::SetD(double value) {
  nt::SetEntryValue(m_d, nt::NetworkTableValue::MakeDouble(value));
}

void NTPIDControllerModel::SetSetpoint(double value) {
  nt::SetEntryValue(m_setpoint, nt::NetworkTableValue::MakeDouble(value));
}

void NTPIDControllerModel::Update() {
  for (auto&& event : m_nt.PollListener()) {
    if (event.entry == m_name) {
      if (event.value && event.value->IsString()) {
        m_nameValue = event.value->GetString();
      }
    } else if (event.entry == m_p) {
      if (event.value && event.value->IsDouble()) {
        m_pData.SetValue(event.value->GetDouble());
      }
    } else if (event.entry == m_i) {
      if (event.value && event.value->IsDouble()) {
        m_iData.SetValue(event.value->GetDouble());
      }
    } else if (event.entry == m_d) {
      if (event.value && event.value->IsDouble()) {
        m_dData.SetValue(event.value->GetDouble());
      }
    } else if (event.entry == m_setpoint) {
      if (event.value && event.value->IsDouble()) {
        m_setpointData.SetValue(event.value->GetDouble());
      }
    } else if (event.entry == m_controllable) {
      if (event.value && event.value->IsBoolean()) {
        m_controllableValue = event.value->GetBoolean();
      }
    }
  }
}

bool NTPIDControllerModel::Exists() {
  return m_nt.IsConnected() && nt::GetEntryType(m_setpoint) != NT_UNASSIGNED;
}
