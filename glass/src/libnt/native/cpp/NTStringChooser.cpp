/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "glass/networktables/NTStringChooser.h"

using namespace glass;

NTStringChooserModel::NTStringChooserModel(wpi::StringRef path)
    : NTStringChooserModel{nt::GetDefaultInstance(), path} {}

NTStringChooserModel::NTStringChooserModel(NT_Inst inst, wpi::StringRef path)
    : m_nt{inst},
      m_default{m_nt.GetEntry(path + "/default")},
      m_selected{m_nt.GetEntry(path + "/selected")},
      m_active{m_nt.GetEntry(path + "/active")},
      m_options{m_nt.GetEntry(path + "/options")} {
  m_nt.AddListener(m_default);
  m_nt.AddListener(m_selected);
  m_nt.AddListener(m_active);
  m_nt.AddListener(m_options);

  Update();
}

void NTStringChooserModel::SetDefault(wpi::StringRef val) {
  nt::SetEntryValue(m_default, nt::Value::MakeString(val));
}

void NTStringChooserModel::SetSelected(wpi::StringRef val) {
  nt::SetEntryValue(m_selected, nt::Value::MakeString(val));
}

void NTStringChooserModel::SetActive(wpi::StringRef val) {
  nt::SetEntryValue(m_active, nt::Value::MakeString(val));
}

void NTStringChooserModel::SetOptions(wpi::ArrayRef<std::string> val) {
  nt::SetEntryValue(m_options, nt::Value::MakeStringArray(val));
}

void NTStringChooserModel::Update() {
  for (auto&& event : m_nt.PollListener()) {
    if (event.entry == m_default && event.value && event.value->IsString()) {
      m_defaultValue = event.value->GetString();
    } else if (event.entry == m_selected && event.value &&
               event.value->IsString()) {
      m_selectedValue = event.value->GetString();
    } else if (event.entry == m_active && event.value &&
               event.value->IsString()) {
      m_activeValue = event.value->GetString();
    } else if (event.entry == m_options && event.value &&
               event.value->IsStringArray()) {
      m_optionsValue = event.value->GetStringArray();
    }
  }
}

bool NTStringChooserModel::Exists() {
  return m_nt.IsConnected() && nt::GetEntryType(m_options) != NT_UNASSIGNED;
}
