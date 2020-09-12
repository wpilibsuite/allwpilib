/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "glass/networktables/NTFMS.h"

#include <stdint.h>

#include <wpi/Twine.h>
#include <wpi/timestamp.h>

using namespace glass;

NTFMSModel::NTFMSModel(wpi::StringRef path)
    : NTFMSModel{nt::GetDefaultInstance(), path} {}

NTFMSModel::NTFMSModel(NT_Inst inst, wpi::StringRef path)
    : m_nt{inst},
      m_gameSpecificMessage{m_nt.GetEntry(path + "/GameSpecificMessage")},
      m_alliance{m_nt.GetEntry(path + "/IsRedAlliance")},
      m_station{m_nt.GetEntry(path + "/StationNumber")},
      m_controlWord{m_nt.GetEntry(path + "/FMSControlData")},
      m_fmsAttached{"NT_FMS:FMSAttached:" + path},
      m_dsAttached{"NT_FMS:DSAttached:" + path},
      m_allianceStationId{"NT_FMS:AllianceStationID:" + path},
      m_estop{"NT_FMS:EStop:" + path},
      m_enabled{"NT_FMS:RobotEnabled:" + path},
      m_test{"NT_FMS:TestMode:" + path},
      m_autonomous{"NT_FMS:AutonomousMode:" + path} {
  m_nt.AddListener(m_alliance);
  m_nt.AddListener(m_station);
  m_nt.AddListener(m_controlWord);

  m_fmsAttached.SetDigital(true);
  m_dsAttached.SetDigital(true);
  m_estop.SetDigital(true);
  m_enabled.SetDigital(true);
  m_test.SetDigital(true);
  m_autonomous.SetDigital(true);
  Update();
}

wpi::StringRef NTFMSModel::GetGameSpecificMessage(
    wpi::SmallVectorImpl<char>& buf) {
  buf.clear();
  auto value = nt::GetEntryValue(m_gameSpecificMessage);
  if (value && value->IsString()) {
    auto str = value->GetString();
    buf.append(str.begin(), str.end());
  }
  return wpi::StringRef{buf.data(), buf.size()};
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
