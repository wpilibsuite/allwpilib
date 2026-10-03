// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/glass/networktables/NTDS.hpp"

#include <stdint.h>

#include <format>
#include <utility>

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

NTDSModel::NTDSModel(std::string_view path)
    : NTDSModel{wpi::nt::NetworkTableInstance::GetDefault(), path} {}

NTDSModel::NTDSModel(wpi::nt::NetworkTableInstance inst, std::string_view path)
    : m_inst{inst},
      m_gameDataSubscriber{
          inst.GetStringTopic(std::format("{}/GameData", path)).Subscribe("")},
      m_allianceStation{
          inst.GetIntegerTopic(std::format("{}/AllianceStationID", path))
              .Subscribe(0)},
      m_controlWord{inst.GetRawTopic(std::format("{}/ControlWord", path))
                        .Subscribe("struct:ControlWord", {})},
      m_fmsAttached{std::format("NT_DS:FMSAttached:{}", path)},
      m_dsAttached{std::format("NT_DS:DSAttached:{}", path)},
      m_allianceStationId{std::format("NT_DS:AllianceStationID:{}", path)},
      m_estop{std::format("NT_DS:EStop:{}", path)},
      m_enabled{std::format("NT_DS:RobotEnabled:{}", path)},
      m_robotMode{std::format("NT_DS:RobotMode:{}", path)},
      m_gameData{std::format("NT_DS:GameData:{}", path)} {}

void NTDSModel::Update() {
  for (auto&& v : m_allianceStation.ReadQueue()) {
    m_allianceStationId.SetValue(v.value, v.time);
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
  for (auto&& v : m_gameDataSubscriber.ReadQueue()) {
    m_gameData.SetValue(std::move(v.value), v.time);
  }
}

bool NTDSModel::Exists() {
  return m_controlWord.Exists();
}
