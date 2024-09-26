// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NTDifferentialDrive.h"

#include <utility>

#include <fmt/format.h>
#include <imgui.h>
#include <wpi/MathExtras.h>
#include <wpi/StringExtras.h>

using namespace glass;

NTDifferentialDriveModel::NTDifferentialDriveModel(std::string_view path)
    : NTDifferentialDriveModel(nt::NetworkTableInstance::GetDefault(), path) {}

NTDifferentialDriveModel::NTDifferentialDriveModel(
    nt::NetworkTableInstance inst, std::string_view path)
    : m_inst{inst},
      m_name{inst.GetStringTopic(fmt::format("{}/.name", path)).Subscribe("")},
      m_controllable{inst.GetBooleanTopic(fmt::format("{}/.controllable", path))
                         .Subscribe(false)},
      m_lPercent{inst.GetDoubleTopic(fmt::format("{}/Left Motor Speed", path))
                     .GetEntry(0)},
      m_rPercent{inst.GetDoubleTopic(fmt::format("{}/Right Motor Speed", path))
                     .GetEntry(0)},
      m_nameValue{wpi::rsplit(path, '/').second},
      m_lPercentData{fmt::format("NTDiffDriveL:{}", path)},
      m_rPercentData{fmt::format("NTDiffDriveR:{}", path)} {
  m_wheels.emplace_back("L % Output", &m_lPercentData,
                        [this](auto value) { m_lPercent.Set(value); });

  m_wheels.emplace_back("R % Output", &m_rPercentData,
                        [this](auto value) { m_rPercent.Set(value); });
}

void NTDifferentialDriveModel::Update() {
  for (auto&& v : m_name.ReadQueue()) {
    m_nameValue = std::move(v.value);
  }
  for (auto&& v : m_lPercent.ReadQueue()) {
    m_lPercentData.SetValue(v.value, v.time);
  }
  for (auto&& v : m_rPercent.ReadQueue()) {
    m_rPercentData.SetValue(v.value, v.time);
  }
  for (auto&& v : m_controllable.ReadQueue()) {
    m_controllableValue = v.value;
  }

  double l = m_lPercentData.GetValue();
  double r = m_rPercentData.GetValue();

  m_speedVector = ImVec2(0.0, -(l + r) / 2.0);
  m_rotation = (l - r) / 2.0;
}

bool NTDifferentialDriveModel::Exists() {
  return m_lPercent.Exists();
}
