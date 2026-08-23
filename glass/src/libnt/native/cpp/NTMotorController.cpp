// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/glass/networktables/NTMotorController.hpp"

#include <format>

#include "wpi/glass/networktables/NTTunableTopic.hpp"
#include "wpi/util/StringExtras.hpp"

using namespace wpi::glass;

NTMotorControllerModel::NTMotorControllerModel(std::string_view path)
    : NTMotorControllerModel(wpi::nt::NetworkTableInstance::GetDefault(),
                             path) {}

NTMotorControllerModel::NTMotorControllerModel(
    wpi::nt::NetworkTableInstance inst, std::string_view path)
    : m_inst{inst},
      m_value{inst.GetDoubleTopic(std::format("{}/Value", path)).GetEntry(0)},
      m_valueData{std::format("NT_SpdCtrl:{}", path)},
      m_nameValue{wpi::util::rsplit(path, '/').second} {}

void NTMotorControllerModel::SetPercent(double value) {
  m_value.Set(value);
}

void NTMotorControllerModel::Update() {
  for (auto&& v : m_value.ReadQueue()) {
    m_valueData.SetValue(v.value, v.time);
  }
}

bool NTMotorControllerModel::Exists() {
  return m_value.Exists();
}

bool NTMotorControllerModel::IsReadOnly() {
  return !IsTunableTopicMutable(m_value.GetTopic());
}
