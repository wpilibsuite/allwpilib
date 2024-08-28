// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <functional>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

#include <wpi/Logger.h>
#include <wpi/print.h>
#include <wpi/struct/Struct.h>
#include <wpinet/UDPClient.h>

#include "ntcore_cpp.h"

namespace wpi {

class TimeSyncServer {
  wpi::Logger m_logger;
  std::function<uint64_t()> m_timeProvider;
  UDPClient m_udp;

  std::atomic_bool m_running;
  std::thread m_listener;

 private:
  void Go();

 public:
  TimeSyncServer(int port = 5810,
                 std::function<uint64_t()> timeProvider = nt::Now);

  void Start();
  void Stop();
};

class TimeSyncClient {
  wpi::Logger m_logger;
  std::function<uint64_t()> m_timeProvider;
  UDPClient m_udp;

  std::string m_serverIP;
  int m_serverPort;
  std::chrono::milliseconds m_loopDelay;

  std::atomic_bool m_running;
  std::thread m_listener;

  int64_t m_offset{0};
  std::mutex m_offsetMutex;

 private:
  void Go(std::chrono::milliseconds loop_time);

 public:
  TimeSyncClient(std::string_view server, int remote_port,
                 std::chrono::milliseconds ping_delay,
                 std::function<uint64_t()> timeProvider = nt::Now);

  void Start();
  void Stop();
  int64_t GetOffset();
};

}  // namespace wpi
