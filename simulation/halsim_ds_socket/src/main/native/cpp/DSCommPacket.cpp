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

#include <FRC_NetworkCommunication/FRCComm.h>
#include <MockData/DriverStationData.h>

/*----------------------------------------------------------------------------
**  The following methods help parse and hold information about the
**   driver station and it's joysticks.
**--------------------------------------------------------------------------*/
void DSCommPacket::SetIndex(uint8_t hi, uint8_t lo) {
  m_hi = hi;
  m_lo = lo;
}

void DSCommPacket::GetIndex(uint8_t& hi, uint8_t& lo) {
  hi = m_hi;
  lo = m_lo;
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

void DSCommPacket::GetControl(uint8_t& control) { control = m_control_sent; }

void DSCommPacket::GetStatus(uint8_t& status) { status = kRobotHasCode; }

void DSCommPacket::SetAlliance(uint8_t station_code) {
  m_alliance_station = static_cast<enum AllianceStationID_t>(station_code);
}

int DSCommPacket::AddDSCommJoystickPacket(uint8_t* data, int len) {
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

void DSCommPacket::GetControlWord(struct ControlWord_t& control_word) {
  control_word = m_control_word;
}

void DSCommPacket::GetAllianceStation(
    enum AllianceStationID_t& alliance_station) {
  alliance_station = m_alliance_station;
}

/*----------------------------------------------------------------------------
**  Communication methods
**--------------------------------------------------------------------------*/
int DSCommPacket::DecodeTCP(uint8_t* packet, int len) {
  if (len < 2) return 0;
  if (packet[0] == 0 && packet[1] == 0) return 2;
  int packet_len = packet[1];
  if (packet_len + 2 > len) return 0;
  int packet_type = static_cast<int>(packet[2]);

  Lock();
  if (packet_type == kGameDataType) {
    m_game_data =
        std::string(reinterpret_cast<char*>(packet + 3), packet_len - 1);
  } else if (packet_type == kJoystickNameType && len >= 7) {
    int joystick = static_cast<int>(packet[3]);
    if (joystick < kMaxJoysticks) {
      m_joystick_types[joystick] = static_cast<int>(packet[5]);
      int namelen = static_cast<int>(packet[6]);
      m_joystick_names[joystick] =
          std::string(reinterpret_cast<char*>(packet + 7), namelen);
    }
  } else {
    std::cerr << "TCP packet type " << packet_type << " unimplemented"
              << std::endl;
    for (int i = 0; i < packet_len + 2; i++)
      std::fprintf(stderr, "%02x ", packet[i]);
    std::fprintf(stderr, "\n");
  }
  Unlock();
  return packet_len + 2;
}

void DSCommPacket::DecodeUDP(uint8_t* packet, int len) {
  if (len < 3) return;

  Lock();
  m_joystick_packets.clear();
  SetIndex(packet[0], packet[1]);
  if (packet[2] != 0) {
    if (len >= 6) {
      SetControl(packet[3], packet[4]);
      SetAlliance(packet[5]);
      packet += 6;
      len -= 6;
      while (len > 0) {
        int packet_len = *packet++;
        if (packet_len > len) break;
        if (*packet == kTagDsCommJoystick) {
          if (AddDSCommJoystickPacket(packet + 1, packet_len - 1) < 0) break;
        }
        packet += packet_len;
        len -= packet_len;
      }
    }
  }
  m_udp_packets++;
  Unlock();
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

    /* TODO(jwhite@codeweavers.com):  If we want to support rumble, etc,
            implement SetJoyStickOutputs, although that would be a callback  */
  }
}

void DSCommPacket::SendTCPToHALSim(void) {
  struct HAL_MatchInfo info;
  Lock();
  info.eventName = strdup("Simulation");
  info.matchType = HAL_MatchType::HAL_kMatchType_none;
  info.matchNumber = 1;
  info.replayNumber = 0;
  info.gameSpecificMessage = strdup(m_game_data.c_str());
  HALSIM_SetMatchInfo(&info);
  std::free(info.gameSpecificMessage);
  std::free(info.eventName);
  Unlock();
}

void DSCommPacket::SendUDPToHALSim(void) {
  struct ControlWord_t control_word;
  AllianceStationID_t alliance_station;

  Lock();
  GetControlWord(control_word);
  GetAllianceStation(alliance_station);
  auto now = std::chrono::high_resolution_clock::now();
  if (m_udp_packets == 1) {
    m_match_time = 0.0;
  } else if (control_word.enabled) {
    std::chrono::duration<double> delta = (now - m_packet_time);
    m_match_time += delta.count();
  }
  m_packet_time = now;
  SendJoysticks();
  Unlock();

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
