// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NTDigitalOutput.h"

#include <fmt/format.h>

using namespace glass;

NTDigitalOutputModel::NTDigitalOutputModel(std::string_view path)
    : NTDigitalOutputModel{nt::GetDefaultInstance(), path} {}

NTDigitalOutputModel::NTDigitalOutputModel(NT_Inst inst, std::string_view path)
    : m_nt{inst},
      m_value{m_nt.GetEntry(fmt::format("{}/Value", path))},
      m_name{m_nt.GetEntry(fmt::format("{}/.name", path))},
      m_controllable{m_nt.GetEntry(fmt::format("{}/.controllable", path))},
      m_valueData{fmt::format("NT_DOut:{}", path)} {
  m_nt.AddListener(m_value);
  m_nt.AddListener(m_name);
  m_nt.AddListener(m_controllable);

  m_valueData.SetDigital(true);
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
