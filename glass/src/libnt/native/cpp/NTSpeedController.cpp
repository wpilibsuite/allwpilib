/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "glass/networktables/NTSpeedController.h"

using namespace glass;

NTSpeedControllerModel::NTSpeedControllerModel(wpi::StringRef path)
    : NTSpeedControllerModel(nt::GetDefaultInstance(), path) {}

NTSpeedControllerModel::NTSpeedControllerModel(NT_Inst instance,
                                               wpi::StringRef path)
    : m_nt(instance),
      m_value(m_nt.GetEntry(path + "/Value")),
      m_name(m_nt.GetEntry(path + "/.name")),
      m_valueData("NT_SpdCtrl:" + path),
      m_nameValue(path.rsplit('/').second) {
  m_nt.AddListener(m_value);
  m_nt.AddListener(m_name);
  Update();
}

void NTSpeedControllerModel::SetPercent(double value) {
  nt::SetEntryValue(m_value, nt::NetworkTableValue::MakeDouble(value));
}

void NTSpeedControllerModel::Update() {
  for (auto&& event : m_nt.PollListener()) {
    if (event.entry == m_value) {
      if (event.value && event.value->IsDouble())
        m_valueData.SetValue(event.value->GetDouble());
    } else if (event.entry == m_name) {
      if (event.value && event.value->IsString())
        m_nameValue = event.value->GetString();
    }
  }
}

bool NTSpeedControllerModel::Exists() {
  return m_nt.IsConnected() && nt::GetEntryType(m_value) != NT_UNASSIGNED;
}
