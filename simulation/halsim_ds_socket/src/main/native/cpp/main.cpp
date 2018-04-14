/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
**  This extension reimplements enough of the FRC_Network layer to enable the
**    simulator to communicate with a driver station.  That includes a
**    simple udp layer for communication.
**  The protocol does not appear to be well documented; this implementation
**    is based in part on the Toast ds_comms.cpp by Jaci and in part
**    by the protocol specification given by QDriverStation.
**--------------------------------------------------------------------------*/

#include <sys/types.h>

#include <cstring>
#include <iostream>

#if defined(Win32) || defined(_WIN32)
#include <windows.h>
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include <DSCommPacket.h>

/*----------------------------------------------------------------------------
**  Open a socket and listen for connections
**    Returns socket handle on success, -1 on error
**--------------------------------------------------------------------------*/
static int OpenListenSocket(int port, bool tcp) {
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

/*----------------------------------------------------------------------------
**  TCP thread
**    This thread only dies at program shut down; it opens a socket
**  and listens for incoming connections, and then processes data
**  sent on the socket.
**--------------------------------------------------------------------------*/
static void TCPThreadFunc(DSCommPacket* ds) {
  static const int kTCPPort = 1740;
  while (true) {
    int socket = OpenListenSocket(kTCPPort, true);
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

        do {
          int deduct = ds->DecodeTCP(buf, len);
          if (deduct <= 0) break;
          std::memmove(buf, buf + deduct, len - deduct);
          len -= deduct;
          if (deduct > 2) ds->SendTCPToHALSim();
        } while (true);
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

/*----------------------------------------------------------------------------
**  Send a reply packet back to the DS
**--------------------------------------------------------------------------*/
static void SendReplyPacket(int socket, struct sockaddr* addr, int addrlen,
                            int reply_port, DSCommPacket* ds) {
  static const uint8_t kTagGeneral = 0x01;
  struct sockaddr_in* in4 = reinterpret_cast<struct sockaddr_in*>(addr);
  in4->sin_port = htons(reply_port);

  uint8_t data[8];
  std::memset(data, 0, sizeof(data));

  ds->GetIndex(data[0], data[1]);

  data[2] = kTagGeneral;
  ds->GetControl(data[3]);
  ds->GetStatus(data[4]);

  data[5] = 12;  // Voltage upper
  data[6] = 0;   // Voltage lower
  data[7] = 0;   // Request

#if defined(WIN32) || defined(_WIN32)
  sendto(socket, reinterpret_cast<const char*>(data), sizeof(data), 0, addr,
         addrlen);
#else
  sendto(socket, data, sizeof(data), 0, addr, addrlen);
#endif
}

/*----------------------------------------------------------------------------
**  UDP thread
**    This thread only dies at program shut down; it opens a socket
**  and listens for incoming connections, and then processes data
**  sent on the socket.
**--------------------------------------------------------------------------*/
static void UDPThreadFunc(DSCommPacket* ds) {
  static const int kUDPListenPort = 1110;
  static const int kUDPReplyPort = 1150;
  while (true) {
    int socket = OpenListenSocket(kUDPListenPort, false);
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
        ds->DecodeUDP(buf, rc);
        SendReplyPacket(socket, &addr, addrlen, kUDPReplyPort, ds);
        ds->SendUDPToHALSim();
      } else {
        break;
      }
    } while (true);
  }
}

/*----------------------------------------------------------------------------
** Main entry point.  We will start listen threads going, processing
**  against our driver station packet
**--------------------------------------------------------------------------*/
extern "C" {
#if defined(WIN32) || defined(_WIN32)
__declspec(dllexport)
#endif
    int HALSIM_InitExtension(void) {
  static DSCommPacket ds;
  static std::thread udp_thread;
  static std::thread tcp_thread;
  static bool once = false;

  if (once) {
    std::cerr << "Error: cannot invoke HALSIM_InitExtension twice."
              << std::endl;
    return -1;
  }
  once = true;

  std::cout << "DriverStationSocket Initializing." << std::endl;

  udp_thread = std::thread(UDPThreadFunc, &ds);
  udp_thread.detach();

  tcp_thread = std::thread(TCPThreadFunc, &ds);
  tcp_thread.detach();

  std::cout << "DriverStationSocket Initialized!" << std::endl;
  return 0;
}
}  // extern "C"
