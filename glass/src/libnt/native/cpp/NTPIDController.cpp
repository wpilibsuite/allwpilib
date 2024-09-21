// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NTPIDController.h"

#include <utility>

#include <fmt/format.h>
#include <wpi/StringExtras.h>

using namespace glass;

NTPIDControllerModel::NTPIDControllerModel(std::string_view path)
    : NTPIDControllerModel(nt::NetworkTableInstance::GetDefault(), path) {}

NTPIDControllerModel::NTPIDControllerModel(nt::NetworkTableInstance inst,
                                           std::string_view path)
    : m_inst{inst},
      m_name{inst.GetStringTopic(fmt::format("{}/.name", path)).Subscribe("")},
      m_controllable{inst.GetBooleanTopic(fmt::format("{}/.controllable", path))
                         .Subscribe(false)},
      m_p{inst.GetDoubleTopic(fmt::format("{}/p", path)).GetEntry(0)},
      m_i{inst.GetDoubleTopic(fmt::format("{}/i", path)).GetEntry(0)},
      m_d{inst.GetDoubleTopic(fmt::format("{}/d", path)).GetEntry(0)},
      m_setpoint{
          inst.GetDoubleTopic(fmt::format("{}/setpoint", path)).GetEntry(0)},
      m_iZone{inst.GetDoubleTopic(fmt::format("{}/izone", path)).GetEntry(0)},
      m_pData{fmt::format("NTPIDCtrlP:{}", path)},
      m_iData{fmt::format("NTPIDCtrlI:{}", path)},
      m_dData{fmt::format("NTPIDCtrlD:{}", path)},
      m_setpointData{fmt::format("NTPIDCtrlStpt:{}", path)},
      m_iZoneData{fmt::format("NTPIDCtrlIZone:{}", path)},
      m_nameValue{wpi::rsplit(path, '/').second} {}

void NTPIDControllerModel::SetP(double value) {
  m_p.Set(value);
}

void NTPIDControllerModel::SetI(double value) {
  m_i.Set(value);
}

void NTPIDControllerModel::SetD(double value) {
  m_d.Set(value);
}

void NTPIDControllerModel::SetSetpoint(double value) {
  m_setpoint.Set(value);
}
void NTPIDControllerModel::SetIZone(double value) {
  m_iZone.Set(value);
}

void NTPIDControllerModel::Update() {
  for (auto&& v : m_name.ReadQueue()) {
    m_nameValue = std::move(v.value);
  }
  for (auto&& v : m_p.ReadQueue()) {
    m_pData.SetValue(v.value, v.time);
  }
  for (auto&& v : m_i.ReadQueue()) {
    m_iData.SetValue(v.value, v.time);
  }
  for (auto&& v : m_d.ReadQueue()) {
    m_dData.SetValue(v.value, v.time);
  }
  for (auto&& v : m_setpoint.ReadQueue()) {
    m_setpointData.SetValue(v.value, v.time);
  }
  for (auto&& v : m_iZone.ReadQueue()) {
    m_iZoneData.SetValue(v.value, v.time);
  }
  for (auto&& v : m_controllable.ReadQueue()) {
    m_controllableValue = v.value;
  }
}

bool NTPIDControllerModel::Exists() {
  return m_setpoint.Exists();
}
