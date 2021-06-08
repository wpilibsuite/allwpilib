// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NTDigitalInput.h"

#include <fmt/format.h>
#include <wpi/StringExtras.h>

using namespace glass;

NTDigitalInputModel::NTDigitalInputModel(std::string_view path)
    : NTDigitalInputModel{nt::GetDefaultInstance(), path} {}

NTDigitalInputModel::NTDigitalInputModel(NT_Inst inst, std::string_view path)
    : m_nt{inst},
      m_value{m_nt.GetEntry(fmt::format("{}/Value", path))},
      m_name{m_nt.GetEntry(fmt::format("{}/.name", path))},
      m_valueData{fmt::format("NT_DIn:{}", path)},
      m_nameValue{wpi::rsplit(path, '/').second} {
  m_nt.AddListener(m_value);
  m_nt.AddListener(m_name);

  m_valueData.SetDigital(true);
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
