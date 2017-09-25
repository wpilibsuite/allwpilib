/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
**  This file reimplements enough of the FRC_Network layer to enable the
**    simulator to communicate with a driver station.  That includes a
**    simple udp layer for communication.
**  The protocol does not appear to be well documented; this implementation
**    is based in part on the Toast ds_comms.cpp by Jaci and in part
**    by the protocol specification given by QDriverStation.
**--------------------------------------------------------------------------*/

#include <sys/types.h>

#if defined(Win32) || defined(_WIN32)
#include <windows.h>
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include <cstring>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include <FRC_NetworkCommunication/FRCComm.h>

/*----------------------------------------------------------------------------
**  The following classes help parse and hold information about the
**   driver station and it's joysticks.
**--------------------------------------------------------------------------*/
class Joystick {
 public:
  std::vector<int16_t> axes;
  uint8_t button_count;
  uint32_t buttons;
  std::vector<int16_t> povs;
};

class DriverStationPacket {
 public:
  uint16_t index;
  uint8_t control_sent;
  struct ControlWord_t control_word;
  enum AllianceStationID_t alliance_station;
  std::vector<Joystick> joysticks;

  void Lock() { mutex.lock(); }

  void Unlock() { mutex.unlock(); }

  void SetIndex(uint8_t hi, uint8_t lo) { index = (hi << 8) | lo; }

  void GetIndex(uint8_t* hi, uint8_t* lo) {
    *hi = index >> 8;
    *lo = index & 0xFF;
  }

  void SetControl(uint8_t control, uint8_t request) {
    static const uint8_t cTest = 0x01;
    static const uint8_t cEnabled = 0x04;
    static const uint8_t cAutonomous = 0x02;
    static const uint8_t cFMS_Attached = 0x08;
    static const uint8_t cEmergencyStop = 0x80;

    static const uint8_t cRequestNormalMask = 0xF0;

    std::memset(&control_word, 0, sizeof(control_word));
    control_word.enabled = (control & cEnabled) != 0;
    control_word.autonomous = (control & cAutonomous) != 0;
    control_word.test = (control & cTest) != 0;
    control_word.eStop = (control & cEmergencyStop) != 0;
    control_word.fmsAttached = (control & cFMS_Attached) != 0;
    control_word.dsAttached = (request & cRequestNormalMask) != 0;

    control_sent = control;
  }

  void GetControl(uint8_t* control) { *control = control_sent; }

  void SetStatus(uint8_t* status) {
    static const uint8_t cRobotHasCode = 0x20;
    *status = cRobotHasCode;
  }

  void SetAlliance(uint8_t station_code) {
    alliance_station = static_cast<enum AllianceStationID_t>(station_code);
  }

  int AddJoystick(uint8_t* data, int len) {
    Joystick stick;
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

    joysticks.push_back(stick);

    return len;
  }

  void GetControlWord(struct ControlWord_t* control) {
    mutex.lock();
    *control = control_word;
    mutex.unlock();
  }

  void GetAllianceStation(enum AllianceStationID_t* allianceStation) {
    mutex.lock();
    *allianceStation = alliance_station;
    mutex.unlock();
  }

 private:
  std::mutex mutex;
};

/*----------------------------------------------------------------------------
**  Global variables.
**    The assumption is that there is only one instance of this
**  module invoked.  That parallels the behavior of the FRCComm library.
**--------------------------------------------------------------------------*/
static int32_t (*occurFunc_callback)(uint32_t refNum) = NULL;
static int32_t given_refnum = 0;
static DriverStationPacket last_ds_packet;
static int64_t ds_packet_count = 0;

/*----------------------------------------------------------------------------
**  Communications functions
**--------------------------------------------------------------------------*/
static int open_listen_socket(int port, bool tcp) {
#if defined(WIN32) || defined(_WIN32)
  SOCKET s;
#else
  int s;
#endif

  if (tcp)
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  else
    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  if (!s) {
    std::perror("socket");
    return -1;
  }

  int reuse = 1;
#if defined(WIN32) || defined(_WIN32)
  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&reuse),
             sizeof(reuse));
#else
  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, static_cast<void*>(&reuse),
             sizeof(reuse));
#endif

  struct sockaddr_in addr;
  std::memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(s, (struct sockaddr*)&addr, sizeof(addr))) {
    std::perror("bind");
#if defined(WIN32) || defined(_WIN32)
    closesocket(s);
#else
    close(s);
#endif
    return -1;
  }

  if (tcp) {
    if (listen(s, 10)) {
#if defined(WIN32) || defined(_WIN32)
      closesocket(s);
#else
      close(s);
#endif
      return -1;
    }
  }

  return s;
}

static int decode_ds_tcp_packet(uint8_t* packet, int len) {
  if (len < 2) return 0;
  if (packet[0] == 0 && packet[1] == 0) return 2;

  // TODO - implement decode...
  std::cerr << "Received TCP packet type " << static_cast<int>(packet[2])
            << "; not handled yet" << std::endl;
  if (occurFunc_callback) (*occurFunc_callback)(given_refnum);
  return len;
}

static void decode_ds_udp_packet(uint8_t* packet, int len) {
  if (len < 3) return;

  last_ds_packet.Lock();
  last_ds_packet.joysticks.clear();
  last_ds_packet.SetIndex(packet[0], packet[1]);
  if (packet[2] != 0) {
    if (len >= 7) {
      last_ds_packet.SetControl(packet[3], packet[4]);
      last_ds_packet.SetAlliance(packet[5]);
      packet += 6;
      len -= 6;
      while (len > 0) {
        static const uint8_t cTagJoystick = 0x0c;
        int packet_len = *packet++;
        if (packet_len > len) break;
        if (*packet == cTagJoystick) {
          if (last_ds_packet.AddJoystick(packet + 1, packet_len - 1) < 0) break;
        }
        packet += (packet_len - 1);
        len -= packet_len;
      }
    }
  }

  last_ds_packet.Unlock();

  ds_packet_count++;
  if (occurFunc_callback) (*occurFunc_callback)(given_refnum);
}

static void send_reply_packet(int socket, struct sockaddr* addr, int addrlen,
                              int reply_port) {
  static const uint8_t cTagGeneral = 0x01;
  struct sockaddr_in* in4 = reinterpret_cast<struct sockaddr_in*>(addr);
  in4->sin_port = htons(reply_port);

  uint8_t data[8];
  std::memset(data, 0, sizeof(data));

  last_ds_packet.Lock();
  last_ds_packet.GetIndex(&data[0], &data[1]);

  data[2] = cTagGeneral;
  last_ds_packet.GetControl(&data[3]);
  last_ds_packet.SetStatus(&data[4]);

  data[5] = 12;  // Voltage upper
  data[6] = 0;   // Voltage lower
  data[7] = 0;   // Request

  last_ds_packet.Unlock();

#if defined(WIN32) || defined(_WIN32)
  sendto(socket, reinterpret_cast<const char*>(data), sizeof(data), 0, addr,
         addrlen);
#else
  sendto(socket, data, sizeof(data), 0, addr, addrlen);
#endif
}

static void tcp_thread_func() {
  while (true) {
    int socket = open_listen_socket(1740, true);
    if (socket < 0) {
#if defined(WIN32) || defined(_WIN32)
      Sleep(1000);
#else
      sleep(1);
#endif
      continue;
    }
    while (true) {
      int client = accept(socket, NULL, 0);
      if (client < 0) {
#if defined(WIN32) || defined(_WIN32)
        closesocket(socket);
#else
        close(socket);
#endif
        break;
      }

      uint8_t buf[8192];
      int len = 0;
      do {
        int rc = recv(client, reinterpret_cast<char*>(buf + len),
                      sizeof(buf) - len, 0);
        if (rc <= 0) break;
        len += rc;
        int deduct = decode_ds_tcp_packet(buf, len);
        if (deduct > 0) {
          std::memmove(buf, buf + deduct, len - deduct);
          len -= deduct;
        }
      } while (true);

#if defined(WIN32) || defined(_WIN32)
      closesocket(client);
      Sleep(1000);
#else
      close(client);
      sleep(1);
#endif
    }
  }
}

static void udp_thread_func() {
  while (true) {
    int socket = open_listen_socket(1110, false);
    if (socket < 0) {
#if defined(WIN32) || defined(_WIN32)
      Sleep(1000);
#else
      sleep(1);
#endif
      continue;
    }

    do {
      uint8_t buf[1024];
      struct sockaddr addr;
#if defined(Win32) || defined(_WIN32)
      int addrlen = sizeof(addr);
      int rc = recvfrom(socket, reinterpret_cast<char*>(buf), sizeof(buf), 0,
                        &addr, &addrlen);
#else
      socklen_t addrlen = sizeof(addr);
      ssize_t rc = recvfrom(socket, buf, sizeof(buf), 0, &addr, &addrlen);
#endif

      if (rc > 0) {
        decode_ds_udp_packet(buf, rc);
        send_reply_packet(socket, &addr, addrlen, 1150);
      } else {
        break;
      }
    } while (true);
  }
}

static std::thread udp_thread;
static std::thread tcp_thread;
static void start() {
  udp_thread = std::thread(udp_thread_func);
  udp_thread.detach();

  tcp_thread = std::thread(tcp_thread_func);
  tcp_thread.detach();
}

/*----------------------------------------------------------------------------
**  Finally, we come to the public interface of this module.  These
**   entry points were copied from
**     ni-libraries/include/FRC_NetworkCommunication/FRCComm.h
**   and then reimplemented in the code below.
**  Obscurity warning:  we start all of our processing when
**   the simulator calls the 'NetCommRPCProxy_SetOccurFuncPointer'.
**  Everything else flows from there.
**--------------------------------------------------------------------------*/

extern "C" {

int setNewDataOccurRef(uint32_t refnum) {
  given_refnum = refnum;
  return 0;
}

int NetCommRPCProxy_SetOccurFuncPointer(int32_t (*occurFunc)(uint32_t refNum)) {
  if (occurFunc_callback == NULL) start();

  occurFunc_callback = occurFunc;
  return 0;
}

/**
 * Send an error to the Driver Station
 * @param isError true if error, false if warning
 * @param errorCode value of error condition
 * @param isLVCode true if error code is defined in errors.txt, false if not
 * (i.e. made up for C++)
 * @param details error description that contains details such as which resource
 * number caused the failure
 * @param location Source file, function, and line number that the error was
 * generated - optional
 * @param callStack The details about what functions were called through before
 * the error was reported - optional
 * @return 0 on success, other on failure
 */
int FRC_NetworkCommunication_sendError(int isError, int32_t errorCode,
                                       int isLVCode, const char* details,
                                       const char* location,
                                       const char* callStack) {
  static bool once = true;
  if (once) {
    std::cerr << "Unimplemented: " << __func__ << std::endl;
    std::cerr << "  Error: " << details << std::endl;
    std::cerr << "  Location: " << location << std::endl;
    std::cerr << "  Callstack: " << callStack << std::endl;
    once = false;
  }
  return -1;  // TODO - implement
}

/**
 * Send error data to the DS
 * @deprecated This old method is hard to parse on the DS side. It will be
 * removed soon. Use FRC_NetworkCommunication_sendError instead.
 * @param errorData is a cstr of the error message
 * @param errorDataLength is the length of the errorData
 * @param wait_ms is ignored (included for binary compatibility)
 * @return 0 on success, 1 on no DS connection
 */
int setErrorData(const char* errors, int errorsLength, int wait_ms) {
  static bool once = true;
  if (once) {
    std::cerr << "Unimplemented: " << __func__ << std::endl;
    once = false;
  }
  return -1;  // TODO - implement
}

int FRC_NetworkCommunication_getControlWord(struct ControlWord_t* controlWord) {
  if (ds_packet_count == 0) return -1;
  last_ds_packet.GetControlWord(controlWord);
  return 0;
}

int FRC_NetworkCommunication_getAllianceStation(
    enum AllianceStationID_t* allianceStation) {
  if (ds_packet_count == 0) return -1;
  last_ds_packet.GetAllianceStation(allianceStation);
  return 0;
}

int FRC_NetworkCommunication_getMatchTime(float* matchTime) {
  static bool once = true;
  if (once) {
    std::cerr << "Unimplemented: " << __func__ << std::endl;
    once = false;
  }
  return -1;  // TODO - implement
}

int FRC_NetworkCommunication_getJoystickAxes(uint8_t joystickNum,
                                             struct JoystickAxes_t* axes,
                                             uint8_t maxAxes) {
  last_ds_packet.Lock();
  if (joystickNum >= last_ds_packet.joysticks.size()) {
    axes->count = 0;
    last_ds_packet.Unlock();
    return -1;
  }

  Joystick* stick = &last_ds_packet.joysticks[joystickNum];
  uint8_t i;
  for (i = 0; i < maxAxes && i < stick->axes.size(); i++) {
    axes->axes[i] = stick->axes[i];
  }
  axes->count = i;
  last_ds_packet.Unlock();
  return 0;
}

int FRC_NetworkCommunication_getJoystickButtons(uint8_t joystickNum,
                                                uint32_t* buttons,
                                                uint8_t* count) {
  last_ds_packet.Lock();
  if (joystickNum >= last_ds_packet.joysticks.size()) {
    last_ds_packet.Unlock();
    *count = 0;
    return -1;
  }

  Joystick* stick = &last_ds_packet.joysticks[joystickNum];
  *count = stick->button_count;
  *buttons = stick->buttons;
  last_ds_packet.Unlock();
  return 0;
}

int FRC_NetworkCommunication_getJoystickPOVs(uint8_t joystickNum,
                                             struct JoystickPOV_t* povs,
                                             uint8_t maxPOVs) {
  last_ds_packet.Lock();
  if (joystickNum >= last_ds_packet.joysticks.size()) {
    last_ds_packet.Unlock();
    povs->count = 0;
    return -1;
  }

  Joystick* stick = &last_ds_packet.joysticks[joystickNum];
  uint8_t i;
  for (i = 0; i < maxPOVs && i < stick->povs.size(); i++) {
    povs->povs[i] = stick->povs[i];
  }
  povs->count = i;
  last_ds_packet.Unlock();
  return 0;
}

int FRC_NetworkCommunication_setJoystickOutputs(uint8_t joystickNum,
                                                uint32_t hidOutputs,
                                                uint16_t leftRumble,
                                                uint16_t rightRumble) {
  static bool once = true;
  if (once) {
    std::cerr << "Unimplemented: " << __func__ << std::endl;
    once = false;
  }
  return -1;  // TODO - implement
}

int FRC_NetworkCommunication_getJoystickDesc(uint8_t joystickNum,
                                             uint8_t* isXBox, uint8_t* type,
                                             char* name, uint8_t* axisCount,
                                             uint8_t* axisTypes,
                                             uint8_t* buttonCount,
                                             uint8_t* povCount) {
  last_ds_packet.Lock();
  if (joystickNum >= last_ds_packet.joysticks.size()) {
    last_ds_packet.Unlock();
    return -1;
  }

  Joystick* stick = &last_ds_packet.joysticks[joystickNum];

  // TODO - isXBox, type, and name
  *axisCount = stick->axes.size();
  *buttonCount = stick->button_count;
  *povCount = stick->povs.size();
  last_ds_packet.Unlock();
  return 0;
}

int FRC_NetworkCommunication_observeUserProgramStarting(void) {
  static bool once = true;
  if (once) {
    std::cerr << "Unimplemented: " << __func__ << std::endl;
    once = false;
  }
  return -1;  // TODO - implement
}

void FRC_NetworkCommunication_observeUserProgramDisabled(void) {
  static bool once = true;
  if (once) {
    std::cerr << "Unimplemented: " << __func__ << std::endl;
    once = false;
  }
  // TODO - implement
}

void FRC_NetworkCommunication_observeUserProgramAutonomous(void) {
  static bool once = true;
  if (once) {
    std::cerr << "Unimplemented: " << __func__ << std::endl;
    once = false;
  }
  // TODO - implement
}

void FRC_NetworkCommunication_observeUserProgramTeleop(void) {
  static bool once = true;
  if (once) {
    std::cerr << "Unimplemented: " << __func__ << std::endl;
    once = false;
  }
  // TODO - implement
}

void FRC_NetworkCommunication_observeUserProgramTest(void) {
  static bool once = true;
  if (once) {
    std::cerr << "Unimplemented: " << __func__ << std::endl;
    once = false;
  }
  // TODO - implement
}

}  // extern "C"
