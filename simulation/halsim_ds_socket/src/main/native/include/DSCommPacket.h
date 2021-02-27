// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>

#include <DSCommJoystickPacket.h>
#include <hal/simulation/DriverStationData.h>
#include <wpi/ArrayRef.h>
#include <wpi/raw_uv_ostream.h>

class DSCommPacketTest;

namespace halsim {

class DSCommPacket {
  friend class ::DSCommPacketTest;

 public:
  DSCommPacket(void);
  void DecodeTCP(wpi::ArrayRef<uint8_t> packet);
  void DecodeUDP(wpi::ArrayRef<uint8_t> packet);
  void SendUDPToHALSim(void);
  void SetupSendBuffer(wpi::raw_uv_ostream& buf);

  /* TCP Tags */
  static const uint8_t kGameDataTag = 0x0e;
  static const uint8_t kJoystickNameTag = 0x02;
  static const uint8_t kMatchInfoTag = 0x07;

  /* UDP Tags*/
  static const uint8_t kJoystickDataTag = 0x0c;
  static const uint8_t kMatchTimeTag = 0x07;

  /* Control word bits */
  static const uint8_t kTest = 0x01;
  static const uint8_t kEnabled = 0x04;
  static const uint8_t kAutonomous = 0x02;
  static const uint8_t kFMS_Attached = 0x08;
  static const uint8_t kEmergencyStop = 0x80;

  /* Control request bitmask */
  static const uint8_t kRequestNormalMask = 0xF0;

  /* Status bits */
  static const uint8_t kRobotHasCode = 0x20;

 private:
  void SendJoysticks(void);
  void SetControl(uint8_t control, uint8_t request);
  void SetAlliance(uint8_t station_code);
  void SetupSendHeader(wpi::raw_uv_ostream& buf);
  void SetupJoystickTag(wpi::raw_uv_ostream& buf);
  void ReadMatchtimeTag(wpi::ArrayRef<uint8_t> tagData);
  void ReadJoystickTag(wpi::ArrayRef<uint8_t> data, int index);
  void ReadNewMatchInfoTag(wpi::ArrayRef<uint8_t> data);
  void ReadGameSpecificMessageTag(wpi::ArrayRef<uint8_t> data);
  void ReadJoystickDescriptionTag(wpi::ArrayRef<uint8_t> data);

  uint8_t m_hi;
  uint8_t m_lo;
  uint8_t m_control_sent;
  HAL_ControlWord m_control_word;
  HAL_AllianceStationID m_alliance_station;
  HAL_MatchInfo matchInfo;
  std::array<DSCommJoystickPacket, HAL_kMaxJoysticks> m_joystick_packets;
  double m_match_time;
};

}  // namespace halsim
