// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>
#include <span>

#include <DSCommJoystickPacket.h>
#include <hal/simulation/DriverStationData.h>
#include <wpinet/raw_uv_ostream.h>

class DSCommPacketTest;

namespace halsim {

class DSCommPacket {
  friend class ::DSCommPacketTest;

 public:
  DSCommPacket(void);
  void DecodeTCP(std::span<const uint8_t> packet);
  void DecodeUDP(std::span<const uint8_t> packet);
  void SendUDPToHALSim(void);
  void SetupSendBuffer(wpi::raw_uv_ostream& buf);

  /* TCP Tags */
  static const uint8_t GAME_DATA_TAG = 0x0e;
  static const uint8_t JOYSTICK_NAME_TAG = 0x02;
  static const uint8_t MATCH_INFO_TAG = 0x07;

  /* UDP Tags*/
  static const uint8_t JOYSTICK_DATA_TAG = 0x0c;
  static const uint8_t MATCH_TIME_TAG = 0x07;

  /* Control word bits */
  static const uint8_t TEST = 0x01;
  static const uint8_t ENABLED = 0x04;
  static const uint8_t AUTONOMOUS = 0x02;
  static const uint8_t FMS_ATTACHED = 0x08;
  static const uint8_t EMERGENCY_STOP = 0x80;

  /* Control request bitmask */
  static const uint8_t REQUEST_NORMAL_MASK = 0xF0;

  /* Status bits */
  static const uint8_t ROBOT_HAS_CODE = 0x20;

 private:
  void SendJoysticks(void);
  void SetControl(uint8_t control, uint8_t request);
  void SetAlliance(uint8_t station_code);
  void SetupSendHeader(wpi::raw_uv_ostream& buf);
  void SetupJoystickTag(wpi::raw_uv_ostream& buf);
  void ReadMatchtimeTag(std::span<const uint8_t> tagData);
  void ReadJoystickTag(std::span<const uint8_t> data, int index);
  void ReadNewMatchInfoTag(std::span<const uint8_t> data);
  void ReadGameSpecificMessageTag(std::span<const uint8_t> data);
  void ReadJoystickDescriptionTag(std::span<const uint8_t> data);

  uint8_t m_hi;
  uint8_t m_lo;
  uint8_t m_control_sent;
  HAL_ControlWord m_control_word;
  HAL_AllianceStationID m_alliance_station;
  HAL_MatchInfo matchInfo;
  std::array<DSCommJoystickPacket, HAL_MaxJoysticks> m_joystick_packets;
  double m_match_time = -1;
};

}  // namespace halsim
