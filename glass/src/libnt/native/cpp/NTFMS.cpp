// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NTFMS.h"

#include <stdint.h>

#include <fmt/format.h>
#include <wpi/timestamp.h>

using namespace glass;

NTFMSModel::NTFMSModel(std::string_view path)
    : NTFMSModel{nt::GetDefaultInstance(), path} {}

NTFMSModel::NTFMSModel(NT_Inst inst, std::string_view path)
    : m_nt{inst},
      m_gameSpecificMessage{
          m_nt.GetEntry(fmt::format("{}/GameSpecificMessage", path))},
      m_alliance{m_nt.GetEntry(fmt::format("{}/IsRedAlliance", path))},
      m_station{m_nt.GetEntry(fmt::format("{}/StationNumber", path))},
      m_controlWord{m_nt.GetEntry(fmt::format("{}/FMSControlData", path))},
      m_fmsAttached{fmt::format("NT_FMS:FMSAttached:{}", path)},
      m_dsAttached{fmt::format("NT_FMS:DSAttached:{}", path)},
      m_allianceStationId{fmt::format("NT_FMS:AllianceStationID:{}", path)},
      m_estop{fmt::format("NT_FMS:EStop:{}", path)},
      m_enabled{fmt::format("NT_FMS:RobotEnabled:{}", path)},
      m_test{fmt::format("NT_FMS:TestMode:{}", path)},
      m_autonomous{fmt::format("NT_FMS:AutonomousMode:{}", path)} {
  m_nt.AddListener(m_alliance);
  m_nt.AddListener(m_station);
  m_nt.AddListener(m_controlWord);

  m_fmsAttached.SetDigital(true);
  m_dsAttached.SetDigital(true);
  m_estop.SetDigital(true);
  m_enabled.SetDigital(true);
  m_test.SetDigital(true);
  m_autonomous.SetDigital(true);
}

std::string_view NTFMSModel::GetGameSpecificMessage(
    wpi::SmallVectorImpl<char>& buf) {
  buf.clear();
  auto value = nt::GetEntryValue(m_gameSpecificMessage);
  if (value && value->IsString()) {
    auto str = value->GetString();
    buf.append(str.begin(), str.end());
  }
  return std::string_view{buf.data(), buf.size()};
}

void NTFMSModel::Update() {
  for (auto&& event : m_nt.PollListener()) {
    if (event.entry == m_alliance) {
      if (event.value && event.value->IsBoolean()) {
        int allianceStationId = m_allianceStationId.GetValue();
        allianceStationId %= 3;
        // true if red
        allianceStationId += 3 * (event.value->GetBoolean() ? 0 : 1);
        m_allianceStationId.SetValue(allianceStationId);
      }
    } else if (event.entry == m_station) {
      if (event.value && event.value->IsDouble()) {
        int allianceStationId = m_allianceStationId.GetValue();
        bool isRed = (allianceStationId < 3);
        // the NT value is 1-indexed
        m_allianceStationId.SetValue(event.value->GetDouble() - 1 +
                                     3 * (isRed ? 0 : 1));
      }
    } else if (event.entry == m_controlWord) {
      if (event.value && event.value->IsDouble()) {
        uint32_t controlWord = event.value->GetDouble();
        // See HAL_ControlWord definition
        auto time = wpi::Now();
        m_enabled.SetValue(((controlWord & 0x01) != 0) ? 1 : 0, time);
        m_autonomous.SetValue(((controlWord & 0x02) != 0) ? 1 : 0, time);
        m_test.SetValue(((controlWord & 0x04) != 0) ? 1 : 0, time);
        m_estop.SetValue(((controlWord & 0x08) != 0) ? 1 : 0, time);
        m_fmsAttached.SetValue(((controlWord & 0x10) != 0) ? 1 : 0, time);
        m_dsAttached.SetValue(((controlWord & 0x20) != 0) ? 1 : 0, time);
      }
    }
  }
}

bool NTFMSModel::Exists() {
  return m_nt.IsConnected() && nt::GetEntryType(m_controlWord) != NT_UNASSIGNED;
}
