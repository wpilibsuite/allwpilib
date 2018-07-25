/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "DSCommPacket.h"

#include <algorithm>
#include <chrono>
#include <cstring>
#include <iostream>
#include <thread>
#include <vector>

#include <FRCComm.h>
#include <mockdata/DriverStationData.h>
#include <mockdata/MockHooks.h>
#include <wpi/ArrayRef.h>
#include <wpi/Format.h>

using namespace halsim;

/*----------------------------------------------------------------------------
**  The following methods help parse and hold information about the
**   driver station and it's joysticks.
**--------------------------------------------------------------------------*/
void DSCommPacket::SetIndex(uint8_t hi, uint8_t lo) {
  m_hi = hi;
  m_lo = lo;
}

void DSCommPacket::SetControl(uint8_t control, uint8_t request) {
  std::memset(&m_control_word, 0, sizeof(m_control_word));
  m_control_word.enabled = (control & kEnabled) != 0;
  m_control_word.autonomous = (control & kAutonomous) != 0;
  m_control_word.test = (control & kTest) != 0;
  m_control_word.eStop = (control & kEmergencyStop) != 0;
  m_control_word.fmsAttached = (control & kFMS_Attached) != 0;
  m_control_word.dsAttached = (request & kRequestNormalMask) != 0;

  m_control_sent = control;
}

void DSCommPacket::SetAlliance(uint8_t station_code) {
  m_alliance_station = static_cast<enum AllianceStationID_t>(station_code);
}

void DSCommPacket::ReadMatchtimeTag(wpi::ArrayRef<uint8_t> tagData) {
  if (tagData.size() < 6) return;

  uint32_t store = tagData[2] << 24;
  store |= tagData[3] << 16;
  store |= tagData[4] << 8;
  store |= tagData[5];

  float matchTime = *reinterpret_cast<float*>(&store);
  m_match_time = matchTime;
}

void DSCommPacket::ReadJoystickTag(wpi::ArrayRef<uint8_t> dataInput, int index) {
  DSCommJoystickPacket& stick = m_joystick_packets[index];
  stick.ResetUdp();

  if (dataInput.size() == 3) {
     return;
  }

  dataInput = dataInput.slice(2);

  // Read axes
  int axesLength = dataInput[0];
  for (int i = 0; i < axesLength; i++) {
    int8_t value = dataInput[1 + i];
    if (value < 0) {
      stick.axes.axes[i] = value / 128.0;
    } else {
      stick.axes.axes[i] = value / 127.0;
    }
  }
  stick.axes.count = axesLength;

  dataInput = dataInput.slice(1 + axesLength);

  // Read Buttons
  int buttonCount = dataInput[0];
  int numBytes = (buttonCount - 1) / 8 + 1;
  stick.buttons.buttons = 0;
  for (int i = 0; i < numBytes; i++) {
    stick.buttons.buttons |= dataInput[1 + i] << (8 * (i));
  }
  stick.buttons.count = buttonCount;

  dataInput = dataInput.slice(1 + numBytes);

  int povsLength = dataInput[0];
  for (int i = 0; i < povsLength * 2; i+= 2) {
    stick.povs.povs[i] = (dataInput[1 + i] << 8) | dataInput[2 + i];
  }

  stick.povs.count = povsLength;

  return;
}

void DSCommPacket::GetControlWord(struct ControlWord_t* control_word) {
  *control_word = m_control_word;
}

void DSCommPacket::GetAllianceStation(
    enum AllianceStationID_t* alliance_station) {
  *alliance_station = m_alliance_station;
}

/*----------------------------------------------------------------------------
**  Communication methods
**--------------------------------------------------------------------------*/
void DSCommPacket::DecodeTCP(wpi::ArrayRef<uint8_t> packet) {

  // No header
  while (!packet.empty()) {
    int tagLength = packet[0] << 8 | packet[1];
    auto tagPacket = packet.slice(0, tagLength + 2);

    if (tagLength == 0) {
      return;
    }

    switch (packet[2]) {
      case kJoystickNameType:
        ReadJoystickDescriptionTag(tagPacket);
        break;
      case kGameDataType:
        ReadGameSpecificMessageTag(tagPacket);
        break;
      case 0x07:
        ReadNewMatchInfoTag(tagPacket);
        break;
    }
    packet = packet.slice(tagLength + 2);
  }
}

void DSCommPacket::DecodeUDP(wpi::ArrayRef<uint8_t> packet) {
  static constexpr uint8_t kMatchTimeTag = 0x07;
  if (packet.size() < 6) return;
  m_udp_packets++;
  std::unique_lock<wpi::mutex> lock(m_mutex);
  // Decode fixed header
  SetIndex(packet[0], packet[1]);
  // Comm Version is packet 2, ignore
  SetControl(packet[3], packet[4]);
  SetAlliance(packet[5]);

  // Return if packet finished
  if (packet.size() == 6) return;

  // Else, handle tagged data
  packet = packet.slice(6);

  int joystickNum = 0;

  // Loop to handle multiple tags
  while (!packet.empty()) {
    auto tagLength = packet[0];
    auto tagPacket = packet.slice(0, tagLength + 1);

    switch (packet[1]) {
      case kTagDsCommJoystick:
        ReadJoystickTag(tagPacket, joystickNum);
        joystickNum++;
        break;
      case kMatchTimeTag:
        ReadMatchtimeTag(tagPacket);
        break;
    }
    packet = packet.slice(tagLength + 1);
  }
}

  void DSCommPacket::ReadOldMatchInfoTag(wpi::ArrayRef<uint8_t> data) {
    // Size 2 bytes, tag 1 byte
    if (data.size() <= 3) return;

    int nameLength = data[3];
    wpi::SmallVector<char, 128> eventName;
    eventName.reserve(nameLength + 1);
    for (int i = 0; i < nameLength; i++) {
      eventName.emplace_back(data[4 + i]);
    }
    eventName.emplace_back('\0');

    data = data.slice(4 + nameLength);

    if (data.size() < 2) return;

    char matchType = data[0]; // 'P', 'Q', 'E' or '0'
    int matchNumber = data[1];
  }

  void DSCommPacket::ReadNewMatchInfoTag(wpi::ArrayRef<uint8_t> data) {
    // Size 2 bytes, tag 1 byte
    if (data.size() <= 3) return;

    int nameLength = data[3];
    wpi::SmallVector<char, 128> eventName;
    eventName.reserve(nameLength + 1);
    for (int i = 0; i < nameLength; i++) {
      eventName.emplace_back(data[4 + i]);
    }

    eventName.emplace_back('\0');

    data = data.slice(4 + nameLength);

    if (data.size() < 4) return;

    int matchType = data[0]; // None, Practice, Qualification, Elimination, Test
    int matchNumber = (data[1] << 8) | data[2];
    int replayNumber = data[3];
  }

  void DSCommPacket::ReadGameSpecificMessageTag(wpi::ArrayRef<uint8_t> data) {
    // Size 2 bytes, tag 1 byte
    if (data.size() <= 3) return;

    wpi::SmallVector<char, 128> gameMessage;
    int length = ((data[0] << 8) | data[1]) - 1;
    gameMessage.reserve(length);
    for (int i = 0; i < length; i++) {
      gameMessage.emplace_back(data[3 + i]);
    }
  }
  void DSCommPacket::ReadJoystickDescriptionTag(wpi::ArrayRef<uint8_t> data) {
    if (data.size() < 3) return;
    data = data.slice(3);
    int joystickNum = data[0];
    DSCommJoystickPacket& packet = m_joystick_packets[joystickNum];
    packet.ResetTcp();
    packet.descriptor.isXbox = data[1] != 0 ? 1 : 0;
    packet.descriptor.type = data[2];
    int nameLength = std::min((int)data[3], (int)(sizeof(packet.descriptor.name) - 1));
    for (int i = 0; i < nameLength; i++) {
      packet.descriptor.name[i] = data[4 + i];
    }
    data = data.slice(4 + nameLength);
    packet.descriptor.name[nameLength] = '\0';
    int axesCount = data[0];
    packet.descriptor.axisCount = axesCount;
    for (int i = 0; i < axesCount; i++) {
      packet.descriptor.axisTypes[i] = data[1 + i];
    }
    data = data.slice(1 + axesCount);

    packet.descriptor.buttonCount = data[0];
    packet.descriptor.povCount = data[1];
  }

void DSCommPacket::SendJoysticks(void) {
  for (int i = 0; i < kMaxJoysticks; i++) {
    DSCommJoystickPacket& packet = m_joystick_packets[i];
    HALSIM_SetJoystickAxes(i, &packet.axes);
    HALSIM_SetJoystickPOVs(i, &packet.povs);
    HALSIM_SetJoystickButtons(i, &packet.buttons);
    HALSIM_SetJoystickDescriptor(i, &packet.descriptor);
  }
}

void DSCommPacket::SetupSendBuffer(wpi::raw_uv_ostream& buf) {
  SetupSendHeader(buf);
  SetupJoystickTag(buf);
}

void DSCommPacket::SetupSendHeader(wpi::raw_uv_ostream& buf) {
  static constexpr uint8_t kCommVersion = 0x01;

  // High low packet index, comm version
  buf << m_hi << m_lo << kCommVersion;

  // Control word and status check
  buf << m_control_sent
      << static_cast<uint8_t>(HALSIM_GetProgramStarted() ? kRobotHasCode : 0);

  // Battery voltage high and low
  buf << static_cast<uint8_t>(12) << static_cast<uint8_t>(0);

  // Request (Always 0)
  buf << static_cast<uint8_t>(0);
}

void DSCommPacket::SetupJoystickTag(wpi::raw_uv_ostream& buf) {
  static constexpr uint8_t kHIDTag = 0x01;

  // HID tags are sent 1 per device
  int64_t outputs;
  int32_t rightRumble;
  int32_t leftRumble;
  for (int i = 0; i < m_joystick_packets.size(); i++) {
    // Length is 9, 1 tag and 8 data.
    buf << static_cast<uint8_t>(9) << kHIDTag;
    HALSIM_GetJoystickOutputs(i, &outputs, &leftRumble, &rightRumble);
    auto op = static_cast<uint32_t>(outputs);
    auto rr = static_cast<uint16_t>(rightRumble);
    auto lr = static_cast<uint16_t>(leftRumble);
    buf.write((op >> 24 & 0xFF));
    buf.write((op >> 16 & 0xFF));
    buf.write((op >> 8 & 0xFF));
    buf.write((op & 0xFF));
    buf.write((rr >> 8 & 0xFF));
    buf.write((rr & 0xFF));
    buf.write((lr >> 8 & 0xFF));
    buf.write((lr & 0xFF));
  }
}

void DSCommPacket::SendTCPToHALSim(void) {
  struct HAL_MatchInfo info;
  std::strncpy(info.eventName, "Simulation", sizeof(info.eventName));
  info.matchType = HAL_MatchType::HAL_kMatchType_none;
  info.matchNumber = 1;
  info.replayNumber = 0;
    std::copy(m_game_data.begin(), m_game_data.end() , info.gameSpecificMessage);
    info.gameSpecificMessageSize = 64;
  HALSIM_SetMatchInfo(&info);
}

void DSCommPacket::SendUDPToHALSim(void) {
  struct ControlWord_t control_word;
  AllianceStationID_t alliance_station;

  std::unique_lock<wpi::mutex> lock(m_mutex);
  GetControlWord(&control_word);
  GetAllianceStation(&alliance_station);
  SendJoysticks();

  HALSIM_SetDriverStationMatchTime(m_match_time);
  HALSIM_SetDriverStationEnabled(control_word.enabled);
  HALSIM_SetDriverStationAutonomous(control_word.autonomous);
  HALSIM_SetDriverStationTest(control_word.test);
  HALSIM_SetDriverStationEStop(control_word.eStop);
  HALSIM_SetDriverStationFmsAttached(control_word.fmsAttached);
  HALSIM_SetDriverStationDsAttached(control_word.dsAttached);
  HALSIM_SetDriverStationAllianceStationId(
      static_cast<HAL_AllianceStationID>(alliance_station));

  HALSIM_NotifyDriverStationNewData();
}
