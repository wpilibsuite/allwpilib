// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/glass/networktables/NTGyro.hpp"

#include <utility>

#include <fmt/format.h>
#include <wpi/util/StringExtras.hpp>

using namespace wpi::glass;

NTGyroModel::NTGyroModel(std::string_view path)
    : NTGyroModel(wpi::nt::NetworkTableInstance::GetDefault(), path) {}

NTGyroModel::NTGyroModel(wpi::nt::NetworkTableInstance inst,
                         std::string_view path)
    : m_inst{inst},
      m_angle{inst.GetDoubleTopic(fmt::format("{}/Value", path)).Subscribe(0)},
      m_name{inst.GetStringTopic(fmt::format("{}/.name", path)).Subscribe({})},
      m_angleData{fmt::format("NT_Gyro:{}", path)},
      m_nameValue{wpi::util::rsplit(path, '/').second} {}

void NTGyroModel::Update() {
  for (auto&& v : m_name.ReadQueue()) {
    m_nameValue = std::move(v.value);
  }
  for (auto&& v : m_angle.ReadQueue()) {
    m_angleData.SetValue(v.value, v.time);
  }
}

bool NTGyroModel::Exists() {
  return m_angle.Exists();
}
