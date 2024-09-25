// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NTMotorController.h"

#include <utility>

#include <fmt/format.h>
#include <wpi/StringExtras.h>

using namespace glass;

NTMotorControllerModel::NTMotorControllerModel(std::string_view path)
    : NTMotorControllerModel(nt::NetworkTableInstance::GetDefault(), path) {}

NTMotorControllerModel::NTMotorControllerModel(nt::NetworkTableInstance inst,
                                               std::string_view path)
    : m_inst{inst},
      m_value{inst.GetDoubleTopic(fmt::format("{}/Value", path)).GetEntry(0)},
      m_name{inst.GetStringTopic(fmt::format("{}/.name", path)).Subscribe("")},
      m_controllable{inst.GetBooleanTopic(fmt::format("{}/.controllable", path))
                         .Subscribe(false)},
      m_valueData{fmt::format("NT_SpdCtrl:{}", path)},
      m_nameValue{wpi::rsplit(path, '/').second} {}

void NTMotorControllerModel::SetPercent(double value) {
  m_value.Set(value);
}

void NTMotorControllerModel::Update() {
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

bool NTMotorControllerModel::Exists() {
  return m_value.Exists();
}
