// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/glass/networktables/NTProfiledPIDController.hpp"

#include <stdint.h>

#include <array>
#include <format>
#include <span>
#include <string>
#include <utility>

#include "wpi/glass/networktables/NTTunableTopic.hpp"
#include "wpi/nt/ntcore_c.h"
#include "wpi/util/StringExtras.hpp"
#include "wpi/util/struct/Struct.hpp"

using namespace wpi::glass;

namespace {

constexpr size_t CONSTRAINTS_STRUCT_SIZE = 16;
constexpr size_t MAX_VELOCITY_OFFSET = 0;
constexpr size_t MAX_ACCELERATION_OFFSET = 8;
constexpr std::string_view DEFAULT_CONSTRAINTS_TYPE =
    "struct:TrapezoidProfileConstraints";

}  // namespace

NTProfiledPIDControllerModel::NTProfiledPIDControllerModel(
    std::string_view path)
    : NTProfiledPIDControllerModel(wpi::nt::NetworkTableInstance::GetDefault(),
                                   path) {}

NTProfiledPIDControllerModel::NTProfiledPIDControllerModel(
    wpi::nt::NetworkTableInstance inst, std::string_view path)
    : m_inst{inst},
      m_p{inst.GetDoubleTopic(std::format("{}/controller/p", path))
              .GetEntry(0)},
      m_i{inst.GetDoubleTopic(std::format("{}/controller/i", path))
              .GetEntry(0)},
      m_d{inst.GetDoubleTopic(std::format("{}/controller/d", path))
              .GetEntry(0)},
      m_iZone{inst.GetDoubleTopic(std::format("{}/controller/izone", path))
                  .GetEntry(0)},
      m_constraints{inst.GetTopic(std::format("{}/constraints", path))
                        .GenericSubscribe()},
      m_goal{inst.GetDoubleTopic(std::format("{}/goal", path)).GetEntry(0)},
      m_pData{std::format("NTPIDCtrlP:{}", path)},
      m_iData{std::format("NTPIDCtrlI:{}", path)},
      m_dData{std::format("NTPIDCtrlD:{}", path)},
      m_iZoneData{std::format("NTPIDCtrlIZone:{}", path)},
      m_maxVelocityData{std::format("NTPIDCtrlMaxVelo:{}", path)},
      m_maxAccelerationData{std::format("NTPIDCtrlMaxAccel:{}", path)},
      m_goalData{std::format("NTPIDCtrlGoal:{}", path)},
      m_nameValue{wpi::util::rsplit(path, '/').second} {}

void NTProfiledPIDControllerModel::SetP(double value) {
  m_p.Set(value);
}

void NTProfiledPIDControllerModel::SetI(double value) {
  m_i.Set(value);
}

void NTProfiledPIDControllerModel::SetD(double value) {
  m_d.Set(value);
}

void NTProfiledPIDControllerModel::SetMaxVelocity(double value) {
  SetConstraints(value, m_maxAccelerationData.GetValue());
}

void NTProfiledPIDControllerModel::SetMaxAcceleration(double value) {
  SetConstraints(m_maxVelocityData.GetValue(), value);
}

void NTProfiledPIDControllerModel::SetIZone(double value) {
  m_iZone.Set(value);
}

void NTProfiledPIDControllerModel::SetGoal(double value) {
  m_goal.Set(value);
}

void NTProfiledPIDControllerModel::SetConstraints(double maxVelocity,
                                                  double maxAcceleration) {
  auto topic = m_constraints.GetTopic();
  auto info = topic.GetInfo();
  if (info.type != NT_RAW) {
    return;
  }

  std::string typeString = info.type_str.empty()
                               ? std::string{DEFAULT_CONSTRAINTS_TYPE}
                               : std::move(info.type_str);
  if (!m_constraintsPublisher || m_constraintsTypeString != typeString) {
    m_constraintsPublisher = topic.GenericPublish(typeString);
    m_constraintsTypeString = std::move(typeString);
  }

  std::array<uint8_t, CONSTRAINTS_STRUCT_SIZE> data;
  wpi::util::PackStruct<MAX_VELOCITY_OFFSET>(std::span{data}, maxVelocity);
  wpi::util::PackStruct<MAX_ACCELERATION_OFFSET>(std::span{data},
                                                 maxAcceleration);
  m_constraintsPublisher.SetRaw(std::span<const uint8_t>{data});
}

void NTProfiledPIDControllerModel::Update() {
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
  for (auto&& v : m_constraints.ReadQueue()) {
    if (!v.IsRaw()) {
      continue;
    }

    auto value = v.GetRaw();
    if (value.size() != CONSTRAINTS_STRUCT_SIZE) {
      continue;
    }

    m_maxVelocityData.SetValue(
        wpi::util::UnpackStruct<double, MAX_VELOCITY_OFFSET>(value), v.time());
    m_maxAccelerationData.SetValue(
        wpi::util::UnpackStruct<double, MAX_ACCELERATION_OFFSET>(value),
        v.time());
  }
  for (auto&& v : m_goal.ReadQueue()) {
    m_goalData.SetValue(v.value, v.time);
  }
}

bool NTProfiledPIDControllerModel::Exists() {
  return m_p.Exists() || m_constraints.GetTopic().Exists() || m_goal.Exists();
}

bool NTProfiledPIDControllerModel::IsReadOnly() {
  return !IsTunableTopicMutable(m_p.GetTopic()) ||
         !IsTunableTopicMutable(m_i.GetTopic()) ||
         !IsTunableTopicMutable(m_d.GetTopic()) ||
         !IsTunableTopicMutable(m_iZone.GetTopic()) ||
         !IsTunableTopicMutable(m_constraints.GetTopic()) ||
         !IsTunableTopicMutable(m_goal.GetTopic());
}
