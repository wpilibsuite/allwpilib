// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NTFMS.h"

#include <stdint.h>

#include <fmt/format.h>
#include <wpi/SmallVector.h>
#include <wpi/timestamp.h>

using namespace glass;

NTFMSModel::NTFMSModel(std::string_view path)
    : NTFMSModel{nt::NetworkTableInstance::GetDefault(), path} {}

NTFMSModel::NTFMSModel(nt::NetworkTableInstance inst, std::string_view path)
    : m_inst{inst},
      m_gameSpecificMessage{
          inst.GetStringTopic(fmt::format("{}/GameSpecificMessage", path))
              .Subscribe("")},
      m_alliance{inst.GetBooleanTopic(fmt::format("{}/IsRedAlliance", path))
                     .Subscribe(false, {{nt::PubSubOption::SendAll(true)}})},
      m_station{inst.GetIntegerTopic(fmt::format("{}/StationNumber", path))
                    .Subscribe(0, {{nt::PubSubOption::SendAll(true)}})},
      m_controlWord{inst.GetIntegerTopic(fmt::format("{}/FMSControlData", path))
                        .Subscribe(0, {{nt::PubSubOption::SendAll(true)}})},
      m_fmsAttached{fmt::format("NT_FMS:FMSAttached:{}", path)},
      m_dsAttached{fmt::format("NT_FMS:DSAttached:{}", path)},
      m_allianceStationId{fmt::format("NT_FMS:AllianceStationID:{}", path)},
      m_estop{fmt::format("NT_FMS:EStop:{}", path)},
      m_enabled{fmt::format("NT_FMS:RobotEnabled:{}", path)},
      m_test{fmt::format("NT_FMS:TestMode:{}", path)},
      m_autonomous{fmt::format("NT_FMS:AutonomousMode:{}", path)} {
  m_fmsAttached.SetDigital(true);
  m_dsAttached.SetDigital(true);
  m_estop.SetDigital(true);
  m_enabled.SetDigital(true);
  m_test.SetDigital(true);
  m_autonomous.SetDigital(true);
}

std::string_view NTFMSModel::GetGameSpecificMessage(
    wpi::SmallVectorImpl<char>& buf) {
  return m_gameSpecificMessage.Get(buf, "");
}

void NTFMSModel::Update() {
  for (auto&& v : m_alliance.ReadQueue()) {
    int allianceStationId = m_allianceStationId.GetValue();
    allianceStationId %= 3;
    // true if red
    allianceStationId += 3 * (v.value ? 0 : 1);
    m_allianceStationId.SetValue(allianceStationId, v.time);
  }
  for (auto&& v : m_station.ReadQueue()) {
    int allianceStationId = m_allianceStationId.GetValue();
    bool isRed = (allianceStationId < 3);
    // the NT value is 1-indexed
    m_allianceStationId.SetValue(v.value - 1 + 3 * (isRed ? 0 : 1), v.time);
  }
  for (auto&& v : m_controlWord.ReadQueue()) {
    uint32_t controlWord = v.value;
    // See HAL_ControlWord definition
    m_enabled.SetValue(((controlWord & 0x01) != 0) ? 1 : 0, v.time);
    m_autonomous.SetValue(((controlWord & 0x02) != 0) ? 1 : 0, v.time);
    m_test.SetValue(((controlWord & 0x04) != 0) ? 1 : 0, v.time);
    m_estop.SetValue(((controlWord & 0x08) != 0) ? 1 : 0, v.time);
    m_fmsAttached.SetValue(((controlWord & 0x10) != 0) ? 1 : 0, v.time);
    m_dsAttached.SetValue(((controlWord & 0x20) != 0) ? 1 : 0, v.time);
  }
}

bool NTFMSModel::Exists() {
  return m_controlWord.Exists();
}
