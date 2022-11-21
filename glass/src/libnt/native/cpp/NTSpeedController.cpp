// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NTSpeedController.h"

#include <fmt/format.h>
#include <wpi/StringExtras.h>

using namespace glass;

NTSpeedControllerModel::NTSpeedControllerModel(std::string_view path)
    : NTSpeedControllerModel(nt::NetworkTableInstance::GetDefault(), path) {}

NTSpeedControllerModel::NTSpeedControllerModel(nt::NetworkTableInstance inst,
                                               std::string_view path)
    : m_inst{inst},
      m_value{inst.GetDoubleTopic(fmt::format("{}/Value", path))
                  .GetEntry(0, {{nt::PubSubOption::SendAll(true)}})},
      m_name{inst.GetStringTopic(fmt::format("{}/.name", path)).Subscribe("")},
      m_controllable{inst.GetBooleanTopic(fmt::format("{}/.controllable", path))
                         .Subscribe(false)},
      m_valueData{fmt::format("NT_SpdCtrl:{}", path)},
      m_nameValue{wpi::rsplit(path, '/').second} {}

void NTSpeedControllerModel::SetPercent(double value) {
  m_value.Set(value);
}

void NTSpeedControllerModel::Update() {
  for (auto&& v : m_value.ReadQueue()) {
    m_valueData.SetValue(v.value, v.time);
  }
  for (auto&& v : m_name.ReadQueue()) {
    m_nameValue = std::move(v.value);
  }
  for (auto&& v : m_controllable.ReadQueue()) {
    m_controllableValue = v.value;
  }
}

bool NTSpeedControllerModel::Exists() {
  return m_value.Exists();
}
