// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NTStringChooser.h"

#include <fmt/format.h>

using namespace glass;

NTStringChooserModel::NTStringChooserModel(std::string_view path)
    : NTStringChooserModel{nt::GetDefaultInstance(), path} {}

NTStringChooserModel::NTStringChooserModel(NT_Inst inst, std::string_view path)
    : m_nt{inst},
      m_default{m_nt.GetEntry(fmt::format("{}/default", path))},
      m_selected{m_nt.GetEntry(fmt::format("{}/selected", path))},
      m_active{m_nt.GetEntry(fmt::format("{}/active", path))},
      m_options{m_nt.GetEntry(fmt::format("{}/options", path))} {
  m_nt.AddListener(m_default);
  m_nt.AddListener(m_selected);
  m_nt.AddListener(m_active);
  m_nt.AddListener(m_options);
}

void NTStringChooserModel::SetDefault(std::string_view val) {
  nt::SetEntryValue(m_default, nt::Value::MakeString(val));
}

void NTStringChooserModel::SetSelected(std::string_view val) {
  nt::SetEntryValue(m_selected, nt::Value::MakeString(val));
}

void NTStringChooserModel::SetActive(std::string_view val) {
  nt::SetEntryValue(m_active, nt::Value::MakeString(val));
}

void NTStringChooserModel::SetOptions(wpi::span<const std::string> val) {
  nt::SetEntryValue(m_options, nt::Value::MakeStringArray(val));
}

void NTStringChooserModel::Update() {
  for (auto&& event : m_nt.PollListener()) {
    if (event.entry == m_default) {
      if ((event.flags & NT_NOTIFY_DELETE) != 0) {
        m_defaultValue.clear();
      } else if (event.value && event.value->IsString()) {
        m_defaultValue = event.value->GetString();
      }
    } else if (event.entry == m_selected) {
      if ((event.flags & NT_NOTIFY_DELETE) != 0) {
        m_selectedValue.clear();
      } else if (event.value && event.value->IsString()) {
        m_selectedValue = event.value->GetString();
      }
    } else if (event.entry == m_active) {
      if ((event.flags & NT_NOTIFY_DELETE) != 0) {
        m_activeValue.clear();
      } else if (event.value && event.value->IsString()) {
        m_activeValue = event.value->GetString();
      }
    } else if (event.entry == m_options) {
      if ((event.flags & NT_NOTIFY_DELETE) != 0) {
        m_optionsValue.clear();
      } else if (event.value && event.value->IsStringArray()) {
        auto arr = event.value->GetStringArray();
        m_optionsValue.assign(arr.begin(), arr.end());
      }
    }
  }
}

bool NTStringChooserModel::Exists() {
  return m_nt.IsConnected() && nt::GetEntryType(m_options) != NT_UNASSIGNED;
}
