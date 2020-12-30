// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NTMecanumDrive.h"

#include <imgui.h>
#include <wpi/MathExtras.h>

using namespace glass;

NTMecanumDriveModel::NTMecanumDriveModel(wpi::StringRef path)
    : NTMecanumDriveModel(nt::GetDefaultInstance(), path) {}

NTMecanumDriveModel::NTMecanumDriveModel(NT_Inst instance, wpi::StringRef path)
    : m_nt(instance),
      m_name(m_nt.GetEntry(path + "/.name")),
      m_flPercent(m_nt.GetEntry(path + "/Front Left Motor Speed")),
      m_frPercent(m_nt.GetEntry(path + "/Front Right Motor Speed")),
      m_rlPercent(m_nt.GetEntry(path + "/Rear Left Motor Speed")),
      m_rrPercent(m_nt.GetEntry(path + "/Rear Right Motor Speed")),
      m_nameValue(path.rsplit('/').second),
      m_flPercentData("NTMcnmDriveFL:" + path),
      m_frPercentData("NTMcnmDriveFR:" + path),
      m_rlPercentData("NTMcnmDriveRL:" + path),
      m_rrPercentData("NTMcnmDriveRR:" + path) {
  m_nt.AddListener(m_name);
  m_nt.AddListener(m_flPercent);
  m_nt.AddListener(m_frPercent);
  m_nt.AddListener(m_rlPercent);
  m_nt.AddListener(m_rrPercent);

  m_wheels.emplace_back("FL % Output", &m_flPercentData, [this](auto value) {
    nt::SetEntryValue(m_flPercent, nt::NetworkTableValue::MakeDouble(value));
  });

  m_wheels.emplace_back("FR % Output", &m_frPercentData, [this](auto value) {
    nt::SetEntryValue(m_frPercent, nt::NetworkTableValue::MakeDouble(value));
  });

  m_wheels.emplace_back("RL % Output", &m_rlPercentData, [this](auto value) {
    nt::SetEntryValue(m_rlPercent, nt::NetworkTableValue::MakeDouble(value));
  });

  m_wheels.emplace_back("RR % Output", &m_rrPercentData, [this](auto value) {
    nt::SetEntryValue(m_rrPercent, nt::NetworkTableValue::MakeDouble(value));
  });
}

void NTMecanumDriveModel::Update() {
  for (auto&& event : m_nt.PollListener()) {
    if (event.entry == m_name && event.value && event.value->IsString()) {
      m_nameValue = event.value->GetString();
    } else if (event.entry == m_flPercent && event.value &&
               event.value->IsDouble()) {
      m_flPercentData.SetValue(event.value->GetDouble());
    } else if (event.entry == m_frPercent && event.value &&
               event.value->IsDouble()) {
      m_frPercentData.SetValue(event.value->GetDouble());
    } else if (event.entry == m_rlPercent && event.value &&
               event.value->IsDouble()) {
      m_rlPercentData.SetValue(event.value->GetDouble());
    } else if (event.entry == m_rrPercent && event.value &&
               event.value->IsDouble()) {
      m_rrPercentData.SetValue(event.value->GetDouble());
    }
  }

  double fl = m_flPercentData.GetValue();
  double fr = m_frPercentData.GetValue();
  double rl = m_rlPercentData.GetValue();
  double rr = m_rrPercentData.GetValue();

  m_speedVector =
      ImVec2((fl - fr - rl + rr) / 4.0f, -(fl + fr + rl + rr) / 4.0f);
  m_rotation = -(-fl + fr - rl + rr) / 4;
}

bool NTMecanumDriveModel::Exists() {
  return m_nt.IsConnected() && nt::GetEntryType(m_flPercent) != NT_UNASSIGNED;
}
