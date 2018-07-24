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

int DSCommPacket::AddDSCommJoystickPacket(wpi::ArrayRef<uint8_t> dataInput) {
  auto data = dataInput.data();
  auto len = dataInput.size();
  DSCommJoystickPacket stick;
  if (len > 0) {
    int axis_count = *data++;
    len--;
    if (axis_count > len) return -1;
    len -= axis_count;
    for (; axis_count > 0; axis_count--) {
      stick.axes.push_back(*data++);
    }
  }

  if (len > 2) {
    stick.button_count = *data++;
    stick.buttons = (*data++) << 8;
    stick.buttons |= *data++;
    len -= 3;
  }

  if (len > 0) {
    int pov_count = *data++;
    len--;
    if (pov_count * 2 > len) return -1;
    len -= pov_count * 2;
    for (; pov_count > 0; pov_count--) {
      stick.povs.push_back((data[0] << 8) | data[1]);
      data += 2;
    }
  }

  m_joystick_packets.push_back(stick);

  return len;
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
int DSCommPacket::DecodeTCP(wpi::ArrayRef<uint8_t> packetInput) {
  auto packet = packetInput.data();
  auto len = packetInput.size();
  if (len < 2) return 0;
  if (packet[0] == 0 && packet[1] == 0) return 2;
  int packet_len = packet[1];
  if (packet_len + 2 > len) return 0;
  int packet_type = static_cast<int>(packet[2]);

  std::unique_lock<std::mutex> lock(m_mutex);
  if (packet_type == kGameDataType) {
    std::copy(
        packet + 3,
        packet + 3 +
            std::min(static_cast<int>(m_game_data.size()), packet_len - 1),
        m_game_data.begin());
  } else if (packet_type == kJoystickNameType && len >= 7) {
    int joystick = static_cast<int>(packet[3]);
    if (joystick < kMaxJoysticks) {
      m_joystick_types[joystick] = static_cast<int>(packet[5]);
      int namelen = static_cast<int>(packet[6]);
      m_joystick_names[joystick] =
          std::string(reinterpret_cast<const char*>(packet + 7), namelen);
    }
  } else {
    std::cerr << "TCP packet type " << packet_type << " unimplemented"
              << std::endl;
    for (int i = 0; i < packet_len + 2; i++)
      std::fprintf(stderr, "%02x ", packet[i]);
    std::fprintf(stderr, "\n");
  }
  return packet_len + 2;
}

void DSCommPacket::DecodeUDP(wpi::ArrayRef<uint8_t> packet) {
  static constexpr uint8_t kMatchTimeTag = 0x07;
  if (packet.size() < 6) return;
  m_udp_packets++;
  std::unique_lock<std::mutex> lock(m_mutex);
  // Decode fixed header
  m_joystick_packets.clear();
  SetIndex(packet[0], packet[1]);
  // Comm Version is packet 2, ignore
  SetControl(packet[3], packet[4]);
  SetAlliance(packet[5]);

  // Return if packet finished
  if (packet.size() == 6) return;

  // Else, handle tagged data
  packet = packet.slice(6);

  // Loop to handle multiple tags
  while (!packet.empty()) {
    auto tagLength = packet[0];

    switch (packet[1]) {
      case kTagDsCommJoystick:
        AddDSCommJoystickPacket(packet.slice(2, tagLength - 1));
        break;
      case kMatchTimeTag:
        ReadMatchtimeTag(packet.slice(0, tagLength + 1));
        break;
    }
    packet = packet.slice(tagLength + 1);
  }
}

void DSCommPacket::SendJoysticks(void) {
  unsigned int i;

  for (i = 0; i < kMaxJoysticks; i++) {
    struct HAL_JoystickAxes axes;
    struct HAL_JoystickPOVs povs;
    struct HAL_JoystickButtons buttons;
    struct HAL_JoystickDescriptor descriptor;
    int j;

    std::memset(&axes, 0, sizeof(axes));
    std::memset(&povs, 0, sizeof(povs));
    std::memset(&buttons, 0, sizeof(buttons));
    std::memset(&descriptor, 0, sizeof(descriptor));

    if (i < m_joystick_packets.size()) {
      axes.count = std::min(static_cast<int>(m_joystick_packets[i].axes.size()),
                            HAL_kMaxJoystickAxes);
      for (j = 0; j < axes.count; j++) {
        int8_t value = m_joystick_packets[i].axes[j];
        if (value < 0) {
          axes.axes[j] = value / 128.0;
        } else {
          axes.axes[j] = value / 127.0;
        }
      }

      povs.count = std::min(static_cast<int>(m_joystick_packets[i].povs.size()),
                            HAL_kMaxJoystickPOVs);
      for (j = 0; j < povs.count; j++)
        povs.povs[j] = m_joystick_packets[i].povs[j];

      buttons.count = m_joystick_packets[i].button_count;
      buttons.buttons = m_joystick_packets[i].buttons;

      descriptor.axisCount = axes.count;
      descriptor.povCount = povs.count;
      descriptor.buttonCount = buttons.count;
    }
    descriptor.type = m_joystick_types[i];
    m_joystick_names[i].copy(descriptor.name, sizeof(descriptor.name) - 1, 0);

    HALSIM_SetJoystickAxes(i, &axes);
    HALSIM_SetJoystickPOVs(i, &povs);
    HALSIM_SetJoystickButtons(i, &buttons);
    HALSIM_SetJoystickDescriptor(i, &descriptor);
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

  // Setup hid tags
  // Length is 8 * num joysticks connected
  buf << kHIDTag << static_cast<uint8_t>(8 * m_joystick_packets.size());
  int64_t outputs;
  int32_t rightRumble;
  int32_t leftRumble;
  for (int i = 0; i < m_joystick_packets.size(); i++) {
    HALSIM_GetJoystickOutputs(i, &outputs, &leftRumble, &rightRumble);
    buf << static_cast<uint32_t>(outputs) << static_cast<uint16_t>(leftRumble)
        << static_cast<uint16_t>(rightRumble);
  }
}

void DSCommPacket::SendTCPToHALSim(void) {
  struct HAL_MatchInfo info;
  std::unique_lock<std::mutex> lock(m_mutex);
  std::strncpy(info.eventName, "Simulation", sizeof(info.eventName));
  info.matchType = HAL_MatchType::HAL_kMatchType_none;
  info.matchNumber = 1;
  info.replayNumber = 0;
  std::copy(info.gameSpecificMessage,
            info.gameSpecificMessage +
                std::min(sizeof(info.gameSpecificMessage), m_game_data.size()),
            m_game_data.begin());
  HALSIM_SetMatchInfo(&info);
}

void DSCommPacket::SendUDPToHALSim(void) {
  struct ControlWord_t control_word;
  AllianceStationID_t alliance_station;

  std::unique_lock<std::mutex> lock(m_mutex);
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
