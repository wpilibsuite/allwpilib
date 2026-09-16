// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/glass/networktables/NTDigitalOutput.hpp"

#include <format>

#include "wpi/glass/networktables/NTTunableTopic.hpp"

using namespace wpi::glass;

NTDigitalOutputModel::NTDigitalOutputModel(std::string_view path)
    : NTDigitalOutputModel{wpi::nt::NetworkTableInstance::GetDefault(), path} {}

NTDigitalOutputModel::NTDigitalOutputModel(wpi::nt::NetworkTableInstance inst,
                                           std::string_view path)
    : m_inst{inst},
      m_value{
          inst.GetBooleanTopic(std::format("{}/Value", path)).GetEntry(false)},
      m_valueData{std::format("NT_DOut:{}", path)} {}

void NTDigitalOutputModel::SetValue(bool val) {
  m_value.Set(val);
}

void NTDigitalOutputModel::Update() {
  for (auto&& v : m_value.ReadQueue()) {
    m_valueData.SetValue(v.value, v.time);
  }
}

bool NTDigitalOutputModel::Exists() {
  return m_value.Exists();
}

bool NTDigitalOutputModel::IsReadOnly() {
  return !IsTunableTopicMutable(m_value.GetTopic());
}
