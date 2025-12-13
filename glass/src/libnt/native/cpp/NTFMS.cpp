// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/glass/networktables/NTFMS.hpp"

#include <stdint.h>

#include <utility>

#include <fmt/format.h>

#include "wpi/util/Endian.hpp"

// FIXME: use dynamic struct decoding
// Duplicated here from DriverStationTypes.h to avoid HAL dependency
#define HAL_CONTROLWORD_OPMODE_HASH_MASK 0x00FFFFFFFFFFFFFFLL
#define HAL_CONTROLWORD_ROBOT_MODE_MASK 0x0300000000000000LL
#define HAL_CONTROLWORD_ROBOT_MODE_SHIFT 56
#define HAL_CONTROLWORD_ENABLED_MASK 0x0400000000000000LL
#define HAL_CONTROLWORD_ESTOP_MASK 0x0800000000000000LL
#define HAL_CONTROLWORD_FMS_ATTACHED_MASK 0x1000000000000000LL
#define HAL_CONTROLWORD_DS_ATTACHED_MASK 0x2000000000000000LL

using namespace wpi::glass;

NTFMSModel::NTFMSModel(std::string_view path)
    : NTFMSModel{wpi::nt::NetworkTableInstance::GetDefault(), path} {}

NTFMSModel::NTFMSModel(wpi::nt::NetworkTableInstance inst,
                       std::string_view path)
    : m_inst{inst},
      m_gameSpecificMessage{
          inst.GetStringTopic(fmt::format("{}/GameSpecificMessage", path))
              .Subscribe("")},
      m_alliance{inst.GetBooleanTopic(fmt::format("{}/IsRedAlliance", path))
                     .Subscribe(false)},
      m_station{inst.GetIntegerTopic(fmt::format("{}/StationNumber", path))
                    .Subscribe(0)},
      m_controlWord{inst.GetRawTopic(fmt::format("{}/ControlWord", path))
                        .Subscribe("struct:ControlWord", {})},
      m_fmsAttached{fmt::format("NT_FMS:FMSAttached:{}", path)},
      m_dsAttached{fmt::format("NT_FMS:DSAttached:{}", path)},
      m_allianceStationId{fmt::format("NT_FMS:AllianceStationID:{}", path)},
      m_estop{fmt::format("NT_FMS:EStop:{}", path)},
      m_enabled{fmt::format("NT_FMS:RobotEnabled:{}", path)},
      m_robotMode{fmt::format("NT_FMS:RobotMode:{}", path)},
      m_gameSpecificMessageData{
          fmt::format("NT_FMS:GameSpecificMessage:{}", path)} {}

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
    if (v.value.size() != sizeof(uint64_t)) {
      continue;
    }
    uint64_t controlWord = wpi::util::support::endian::read64le(v.value.data());
    // See wpi::Struct<HAL_ControlWord> definition
    m_enabled.SetValue(
        ((controlWord & HAL_CONTROLWORD_ENABLED_MASK) != 0) ? 1 : 0, v.time);
    m_robotMode.SetValue((controlWord & HAL_CONTROLWORD_ROBOT_MODE_MASK) >>
                             HAL_CONTROLWORD_ROBOT_MODE_SHIFT,
                         v.time);
    m_estop.SetValue(((controlWord & HAL_CONTROLWORD_ESTOP_MASK) != 0) ? 1 : 0,
                     v.time);
    m_fmsAttached.SetValue(
        ((controlWord & HAL_CONTROLWORD_FMS_ATTACHED_MASK) != 0) ? 1 : 0,
        v.time);
    m_dsAttached.SetValue(
        ((controlWord & HAL_CONTROLWORD_DS_ATTACHED_MASK) != 0) ? 1 : 0,
        v.time);
  }
  for (auto&& v : m_gameSpecificMessage.ReadQueue()) {
    m_gameSpecificMessageData.SetValue(std::move(v.value), v.time);
  }
}

bool NTFMSModel::Exists() {
  return m_controlWord.Exists();
}
