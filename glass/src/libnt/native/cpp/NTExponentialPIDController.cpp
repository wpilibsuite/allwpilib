// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NTExponentialPIDController.h"

#include <utility>

#include <fmt/format.h>
#include <wpi/StringExtras.h>

using namespace glass;

NTExponentialPIDControllerModel::NTExponentialPIDControllerModel(
    std::string_view path)
    : NTExponentialPIDControllerModel(nt::NetworkTableInstance::GetDefault(),
                                      path) {}

NTExponentialPIDControllerModel::NTExponentialPIDControllerModel(
    nt::NetworkTableInstance inst, std::string_view path)
    : m_inst{inst},
      m_name{inst.GetStringTopic(fmt::format("{}/.name", path)).Subscribe("")},
      m_controllable{inst.GetBooleanTopic(fmt::format("{}/.controllable", path))
                         .Subscribe(false)},
      m_p{inst.GetDoubleTopic(fmt::format("{}/p", path)).GetEntry(0)},
      m_i{inst.GetDoubleTopic(fmt::format("{}/i", path)).GetEntry(0)},
      m_d{inst.GetDoubleTopic(fmt::format("{}/d", path)).GetEntry(0)},
      m_iZone{inst.GetDoubleTopic(fmt::format("{}/izone", path)).GetEntry(0)},
      m_maxInput{
          inst.GetDoubleTopic(fmt::format("{}/maxinput", path)).GetEntry(0)},
      m_a{inst.GetDoubleTopic(fmt::format("{}/a", path)).GetEntry(0)},
      m_b{inst.GetDoubleTopic(fmt::format("{}/b", path)).GetEntry(0)},
      m_goal{inst.GetDoubleTopic(fmt::format("{}/goal", path)).GetEntry(0)},
      m_pData{fmt::format("NTPIDCtrlP:{}", path)},
      m_iData{fmt::format("NTPIDCtrlI:{}", path)},
      m_dData{fmt::format("NTPIDCtrlD:{}", path)},
      m_iZoneData{fmt::format("NTPIDCtrlIZone:{}", path)},
      m_maxInputData{fmt::format("NTPIDCtrlMaxInput:{}", path)},
      m_aData{fmt::format("NTPIDCtrlA:{}", path)},
      m_bData{fmt::format("NTPIDCtrlB:{}", path)},
      m_goalData{fmt::format("NTPIDCtrlGoal:{}", path)},
      m_nameValue{wpi::rsplit(path, '/').second} {}

void NTExponentialPIDControllerModel::SetP(double value) {
  m_p.Set(value);
}

void NTExponentialPIDControllerModel::SetI(double value) {
  m_i.Set(value);
}

void NTExponentialPIDControllerModel::SetD(double value) {
  m_d.Set(value);
}

void NTExponentialPIDControllerModel::SetMaxInput(double value) {
  m_maxInput.Set(value);
}

void NTExponentialPIDControllerModel::SetA(double value) {
  m_a.Set(value);
}

void NTExponentialPIDControllerModel::SetB(double value) {
  m_b.Set(value);
}

void NTExponentialPIDControllerModel::SetIZone(double value) {
  m_iZone.Set(value);
}

void NTExponentialPIDControllerModel::SetGoal(double value) {
  m_goal.Set(value);
}
void NTExponentialPIDControllerModel::Update() {
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
  for (auto&& v : m_iZone.ReadQueue()) {
    m_iZoneData.SetValue(v.value, v.time);
  }
  for (auto&& v : m_maxInput.ReadQueue()) {
    m_maxInputData.SetValue(v.value, v.time);
  }
  for (auto&& v : m_a.ReadQueue()) {
    m_aData.SetValue(v.value, v.time);
  }
  for (auto&& v : m_b.ReadQueue()) {
    m_bData.SetValue(v.value, v.time);
  }
  for (auto&& v : m_goal.ReadQueue()) {
    m_goalData.SetValue(v.value, v.time);
  }
  for (auto&& v : m_controllable.ReadQueue()) {
    m_controllableValue = v.value;
  }
}

bool NTExponentialPIDControllerModel::Exists() {
  return m_goal.Exists();
}
