// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/glass/networktables/NTDigitalInput.hpp"

#include <format>

using namespace wpi::glass;

NTDigitalInputModel::NTDigitalInputModel(std::string_view path)
    : NTDigitalInputModel{wpi::nt::NetworkTableInstance::GetDefault(), path} {}

NTDigitalInputModel::NTDigitalInputModel(wpi::nt::NetworkTableInstance inst,
                                         std::string_view path)
    : m_inst{inst},
      m_value{
          inst.GetBooleanTopic(std::format("{}/Value", path)).Subscribe(false)},
      m_valueData{std::format("NT_DIn:{}", path)} {}

void NTDigitalInputModel::Update() {
  for (auto&& v : m_value.ReadQueue()) {
    m_valueData.SetValue(v.value, v.time);
  }
}

bool NTDigitalInputModel::Exists() {
  return m_value.Exists();
}
