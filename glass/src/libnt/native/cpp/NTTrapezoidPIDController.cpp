// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NTTrapezoidPIDController.h"

#include <utility>

#include <fmt/format.h>
#include <wpi/StringExtras.h>

using namespace glass;

NTTrapezoidPIDControllerModel::NTTrapezoidPIDControllerModel(
    std::string_view path)
    : NTTrapezoidPIDControllerModel(nt::NetworkTableInstance::GetDefault(),
                                   path) {}

NTTrapezoidPIDControllerModel::NTTrapezoidPIDControllerModel(
    nt::NetworkTableInstance inst, std::string_view path)
    : m_inst{inst},
      m_name{inst.GetStringTopic(fmt::format("{}/.name", path)).Subscribe("")},
      m_controllable{inst.GetBooleanTopic(fmt::format("{}/.controllable", path))
                         .Subscribe(false)},
      m_p{inst.GetDoubleTopic(fmt::format("{}/p", path)).GetEntry(0)},
      m_i{inst.GetDoubleTopic(fmt::format("{}/i", path)).GetEntry(0)},
      m_d{inst.GetDoubleTopic(fmt::format("{}/d", path)).GetEntry(0)},
      m_iZone{inst.GetDoubleTopic(fmt::format("{}/izone", path)).GetEntry(0)},
      m_maxVelocity{
          inst.GetDoubleTopic(fmt::format("{}/maxVelocity", path)).GetEntry(0)},
      m_maxAcceleration{
          inst.GetDoubleTopic(fmt::format("{}/maxAcceleration", path))
              .GetEntry(0)},
      m_goal{inst.GetDoubleTopic(fmt::format("{}/goal", path)).GetEntry(0)},
      m_pData{fmt::format("NTPIDCtrlP:{}", path)},
      m_iData{fmt::format("NTPIDCtrlI:{}", path)},
      m_dData{fmt::format("NTPIDCtrlD:{}", path)},
      m_iZoneData{fmt::format("NTPIDCtrlIZone:{}", path)},
      m_maxVelocityData{fmt::format("NTPIDCtrlMaxVelo:{}", path)},
      m_maxAccelerationData{fmt::format("NTPIDCtrlMaxAccel:{}", path)},
      m_goalData{fmt::format("NTPIDCtrlGoal:{}", path)},
      m_nameValue{wpi::rsplit(path, '/').second} {}

void NTTrapezoidPIDControllerModel::SetP(double value) {
  m_p.Set(value);
}

void NTTrapezoidPIDControllerModel::SetI(double value) {
  m_i.Set(value);
}

void NTTrapezoidPIDControllerModel::SetD(double value) {
  m_d.Set(value);
}

void NTTrapezoidPIDControllerModel::SetMaxVelocity(double value) {
  m_maxVelocity.Set(value);
}

void NTTrapezoidPIDControllerModel::SetMaxAcceleration(double value) {
  m_maxAcceleration.Set(value);
}

void NTTrapezoidPIDControllerModel::SetIZone(double value) {
  m_iZone.Set(value);
}

void NTTrapezoidPIDControllerModel::SetGoal(double value) {
  m_goal.Set(value);
}
void NTTrapezoidPIDControllerModel::Update() {
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
  for (auto&& v : m_maxVelocity.ReadQueue()) {
    m_maxVelocityData.SetValue(v.value, v.time);
  }
  for (auto&& v : m_maxAcceleration.ReadQueue()) {
    m_maxAccelerationData.SetValue(v.value, v.time);
  }
  for (auto&& v : m_goal.ReadQueue()) {
    m_goalData.SetValue(v.value, v.time);
  }
  for (auto&& v : m_controllable.ReadQueue()) {
    m_controllableValue = v.value;
  }
}

bool NTTrapezoidPIDControllerModel::Exists() {
  return m_goal.Exists();
}
