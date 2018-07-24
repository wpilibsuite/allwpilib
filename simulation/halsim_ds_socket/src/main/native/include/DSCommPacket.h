/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <chrono>
#include <string>
#include <thread>
#include <array>
#include <wpi/SmallVector.h>

#include <DSCommJoystickPacket.h>
#include <FRCComm.h>
#include <mockdata/DriverStationData.h>
#include <wpi/uv/Buffer.h>
#include <wpi/mutex.h>
#include <wpi/ArrayRef.h>
#include <wpi/timestamp.h>
#include <wpi/raw_uv_ostream.h>

namespace halsim {

int& GetOrResetBufferCount(bool reset);

class DSCommPacket {
 public:
  DSCommPacket(void) {
    m_joystick_types.fill(-1);
    m_packet_time = wpi::Now();
  }
  void SetIndex(uint8_t hi, uint8_t lo);
  void GetIndex(uint8_t* hi, uint8_t* lo);
  void SetControl(uint8_t control, uint8_t request);
  void GetControl(uint8_t* control);
  void GetStatus(uint8_t* status);
  void SetAlliance(uint8_t station_code);
  int AddDSCommJoystickPacket(wpi::ArrayRef<uint8_t> data);
  void GetControlWord(struct ControlWord_t* control_word);
  void GetAllianceStation(enum AllianceStationID_t* allianceStation);
  void GetJoystickOutputs(int64_t* outputs, int32_t* leftRumble, int32_t* rightRumble);
  int DecodeTCP(wpi::ArrayRef<uint8_t> packet);
  void DecodeUDP(wpi::ArrayRef<uint8_t> packet);
  void SendTCPToHALSim(void);
  void SendUDPToHALSim(void);
  void SendJoysticks(void);
  void SetupSendBuffer(wpi::SmallVectorImpl<wpi::uv::Buffer>& buf);

  /* TCP (FMS) types */
  static const uint8_t kGameDataType = 0x0e;
  static const uint8_t kJoystickNameType = 0x02;

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

  /* Joystick tag bits */
  static const uint8_t kTagDsCommJoystick = 0x0c;

  /* Joystick max count */
  /* TODO(jwhite@codeweavers.com) This is a magic number in the HAL; fix it
   * there */
  static const uint8_t kMaxJoysticks = HAL_kMaxJoysticks;

 private:
  std::array<uint8_t, 64> m_game_data;
  uint8_t m_hi;
  uint8_t m_lo;
  uint8_t m_control_sent;
  struct ControlWord_t m_control_word;
  enum AllianceStationID_t m_alliance_station;
  wpi::SmallVector<DSCommJoystickPacket, 6> m_joystick_packets;
  std::array<std::string, kMaxJoysticks> m_joystick_names;
  std::array<int, kMaxJoysticks> m_joystick_types;
  wpi::mutex m_mutex;
  int m_udp_packets;
  uint64_t m_packet_time;
  double m_match_time;
};

}  // namespace halsim
