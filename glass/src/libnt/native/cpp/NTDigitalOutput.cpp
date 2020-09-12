/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "glass/networktables/NTDigitalOutput.h"

#include <wpi/Twine.h>

using namespace glass;

NTDigitalOutputModel::NTDigitalOutputModel(wpi::StringRef path)
    : NTDigitalOutputModel{nt::GetDefaultInstance(), path} {}

NTDigitalOutputModel::NTDigitalOutputModel(NT_Inst inst, wpi::StringRef path)
    : m_nt{inst},
      m_value{m_nt.GetEntry(path + "/Value")},
      m_name{m_nt.GetEntry(path + "/.name")},
      m_controllable{m_nt.GetEntry(path + "/.controllable")},
      m_valueData{"NT_DOut:" + path} {
  m_nt.AddListener(m_value);
  m_nt.AddListener(m_name);
  m_nt.AddListener(m_controllable);

  m_valueData.SetDigital(true);
  Update();
}

void NTDigitalOutputModel::SetValue(bool val) {
  nt::SetEntryValue(m_value, nt::Value::MakeBoolean(val));
}

void NTDigitalOutputModel::Update() {
  for (auto&& event : m_nt.PollListener()) {
    if (event.entry == m_value) {
      if (event.value && event.value->IsBoolean()) {
        m_valueData.SetValue(event.value->GetBoolean());
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

bool NTDigitalOutputModel::Exists() {
  return m_nt.IsConnected() && nt::GetEntryType(m_value) != NT_UNASSIGNED;
}
