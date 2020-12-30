// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NTDifferentialDrive.h"

#include <imgui.h>
#include <wpi/MathExtras.h>

using namespace glass;

NTDifferentialDriveModel::NTDifferentialDriveModel(wpi::StringRef path)
    : NTDifferentialDriveModel(nt::GetDefaultInstance(), path) {}

NTDifferentialDriveModel::NTDifferentialDriveModel(NT_Inst instance,
                                                   wpi::StringRef path)
    : m_nt(instance),
      m_name(m_nt.GetEntry(path + "/.name")),
      m_lPercent(m_nt.GetEntry(path + "/Left Motor Speed")),
      m_rPercent(m_nt.GetEntry(path + "/Right Motor Speed")),
      m_nameValue(path.rsplit('/').second),
      m_lPercentData("NTDiffDriveL:" + path),
      m_rPercentData("NTDiffDriveR:" + path) {
  m_nt.AddListener(m_name);
  m_nt.AddListener(m_lPercent);
  m_nt.AddListener(m_rPercent);

  m_wheels.emplace_back("L % Output", &m_lPercentData, [this](auto value) {
    nt::SetEntryValue(m_lPercent, nt::NetworkTableValue::MakeDouble(value));
  });

  m_wheels.emplace_back("R % Output", &m_rPercentData, [this](auto value) {
    nt::SetEntryValue(m_rPercent, nt::NetworkTableValue::MakeDouble(value));
  });
}

void NTDifferentialDriveModel::Update() {
  for (auto&& event : m_nt.PollListener()) {
    if (event.entry == m_name && event.value && event.value->IsString()) {
      m_nameValue = event.value->GetString();
    } else if (event.entry == m_lPercent && event.value &&
               event.value->IsDouble()) {
      m_lPercentData.SetValue(event.value->GetDouble());
    } else if (event.entry == m_rPercent && event.value &&
               event.value->IsDouble()) {
      m_rPercentData.SetValue(event.value->GetDouble());
    }
  }

  double l = m_lPercentData.GetValue();
  double r = m_rPercentData.GetValue();

  m_speedVector = ImVec2(0.0, -(l + r) / 2.0);
  m_rotation = (l - r) / 2.0;
}

bool NTDifferentialDriveModel::Exists() {
  return m_nt.IsConnected() && nt::GetEntryType(m_lPercent) != NT_UNASSIGNED;
}
