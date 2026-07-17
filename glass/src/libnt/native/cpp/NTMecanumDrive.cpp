// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/glass/networktables/NTMecanumDrive.hpp"

#include <format>
#include <utility>

#include <imgui.h>

#include "wpi/util/StringExtras.hpp"

using namespace wpi::glass;

NTMecanumDriveModel::NTMecanumDriveModel(std::string_view path)
    : NTMecanumDriveModel(wpi::nt::NetworkTableInstance::GetDefault(), path) {}

NTMecanumDriveModel::NTMecanumDriveModel(wpi::nt::NetworkTableInstance inst,
                                         std::string_view path)
    : m_inst{inst},
      m_name{inst.GetStringTopic(std::format("{}/.name", path)).Subscribe("")},
      m_controllable{inst.GetBooleanTopic(std::format("{}/.controllable", path))
                         .Subscribe(0)},
      m_flPercent{
          inst.GetDoubleTopic(std::format("{}/Front Left Motor Velocity", path))
              .GetEntry(0)},
      m_frPercent{inst.GetDoubleTopic(
                          std::format("{}/Front Right Motor Velocity", path))
                      .GetEntry(0)},
      m_rlPercent{
          inst.GetDoubleTopic(std::format("{}/Rear Left Motor Velocity", path))
              .GetEntry(0)},
      m_rrPercent{
          inst.GetDoubleTopic(std::format("{}/Rear Right Motor Velocity", path))
              .GetEntry(0)},
      m_nameValue{wpi::util::rsplit(path, '/').second},
      m_flPercentData{std::format("NTMcnmDriveFL:{}", path)},
      m_frPercentData{std::format("NTMcnmDriveFR:{}", path)},
      m_rlPercentData{std::format("NTMcnmDriveRL:{}", path)},
      m_rrPercentData{std::format("NTMcnmDriveRR:{}", path)} {
  m_wheels.emplace_back("FL % Output", &m_flPercentData,
                        [this](auto value) { m_flPercent.Set(value); });

  m_wheels.emplace_back("FR % Output", &m_frPercentData,
                        [this](auto value) { m_frPercent.Set(value); });

  m_wheels.emplace_back("RL % Output", &m_rlPercentData,
                        [this](auto value) { m_rlPercent.Set(value); });

  m_wheels.emplace_back("RR % Output", &m_rrPercentData,
                        [this](auto value) { m_rrPercent.Set(value); });
}

void NTMecanumDriveModel::Update() {
  for (auto&& v : m_name.ReadQueue()) {
    m_nameValue = std::move(v.value);
  }
  for (auto&& v : m_flPercent.ReadQueue()) {
    m_flPercentData.SetValue(v.value, v.time);
  }
  for (auto&& v : m_frPercent.ReadQueue()) {
    m_frPercentData.SetValue(v.value, v.time);
  }
  for (auto&& v : m_rlPercent.ReadQueue()) {
    m_rlPercentData.SetValue(v.value, v.time);
  }
  for (auto&& v : m_rrPercent.ReadQueue()) {
    m_rrPercentData.SetValue(v.value, v.time);
  }
  for (auto&& v : m_controllable.ReadQueue()) {
    m_controllableValue = v.value;
  }

  double fl = m_flPercentData.GetValue();
  double fr = m_frPercentData.GetValue();
  double rl = m_rlPercentData.GetValue();
  double rr = m_rrPercentData.GetValue();

  m_velocityVector =
      ImVec2((fl - fr - rl + rr) / 4.0f, -(fl + fr + rl + rr) / 4.0f);
  m_rotation = -(-fl + fr - rl + rr) / 4;
}

bool NTMecanumDriveModel::Exists() {
  return m_flPercent.Exists();
}
