// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/glass/networktables/NTDigitalInput.hpp"

#include <utility>

#include <fmt/format.h>

#include "wpi/util/StringExtras.hpp"

using namespace glass;

NTDigitalInputModel::NTDigitalInputModel(std::string_view path)
    : NTDigitalInputModel{nt::NetworkTableInstance::GetDefault(), path} {}

NTDigitalInputModel::NTDigitalInputModel(nt::NetworkTableInstance inst,
                                         std::string_view path)
    : m_inst{inst},
      m_value{
          inst.GetBooleanTopic(fmt::format("{}/Value", path)).Subscribe(false)},
      m_name{inst.GetStringTopic(fmt::format("{}/.name", path)).Subscribe("")},
      m_valueData{fmt::format("NT_DIn:{}", path)},
      m_nameValue{wpi::rsplit(path, '/').second} {}

void NTDigitalInputModel::Update() {
  for (auto&& v : m_value.ReadQueue()) {
    m_valueData.SetValue(v.value, v.time);
  }
  for (auto&& v : m_name.ReadQueue()) {
    m_nameValue = std::move(v.value);
  }
}

bool NTDigitalInputModel::Exists() {
  return m_value.Exists();
}
