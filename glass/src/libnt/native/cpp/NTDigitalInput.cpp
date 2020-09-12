/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "glass/networktables/NTDigitalInput.h"

#include <wpi/Twine.h>

using namespace glass;

NTDigitalInputModel::NTDigitalInputModel(wpi::StringRef path)
    : NTDigitalInputModel{nt::GetDefaultInstance(), path} {}

NTDigitalInputModel::NTDigitalInputModel(NT_Inst inst, wpi::StringRef path)
    : m_nt{inst},
      m_value{m_nt.GetEntry(path + "/Value")},
      m_name{m_nt.GetEntry(path + "/.name")},
      m_valueData{"NT_DIn:" + path},
      m_nameValue{path.rsplit('/').second} {
  m_nt.AddListener(m_value);
  m_nt.AddListener(m_name);

  m_valueData.SetDigital(true);
  Update();
}

void NTDigitalInputModel::Update() {
  for (auto&& event : m_nt.PollListener()) {
    if (event.entry == m_value) {
      if (event.value && event.value->IsBoolean()) {
        m_valueData.SetValue(event.value->GetBoolean());
      }
    } else if (event.entry == m_name) {
      if (event.value && event.value->IsString()) {
        m_nameValue = event.value->GetString();
      }
    }
  }
}

bool NTDigitalInputModel::Exists() {
  return m_nt.IsConnected() && nt::GetEntryType(m_value) != NT_UNASSIGNED;
}
