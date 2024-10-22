// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NTDigitalOutput.h"

#include <utility>

#include <fmt/format.h>

using namespace glass;

NTDigitalOutputModel::NTDigitalOutputModel(std::string_view path)
    : NTDigitalOutputModel{nt::NetworkTableInstance::GetDefault(), path} {}

NTDigitalOutputModel::NTDigitalOutputModel(nt::NetworkTableInstance inst,
                                           std::string_view path)
    : m_inst{inst},
      m_value{
          inst.GetBooleanTopic(fmt::format("{}/Value", path)).GetEntry(false)},
      m_name{inst.GetStringTopic(fmt::format("{}/.name", path)).Subscribe("")},
      m_controllable{inst.GetBooleanTopic(fmt::format("{}/.controllable", path))
                         .Subscribe(false)},
      m_valueData{fmt::format("NT_DOut:{}", path)} {
  m_valueData.SetDigital(true);
}

void NTDigitalOutputModel::SetValue(bool val) {
  m_value.Set(val);
}

void NTDigitalOutputModel::Update() {
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

bool NTDigitalOutputModel::Exists() {
  return m_value.Exists();
}
