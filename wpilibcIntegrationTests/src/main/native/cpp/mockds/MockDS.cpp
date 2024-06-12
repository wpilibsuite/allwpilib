// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "MockDS.h"

#include <stdint.h>

#include <string_view>

#include <hal/cpp/fpga_clock.h>
#include <wpi/Logger.h>
#include <wpi/SmallVector.h>
#include <wpi/print.h>
#include <wpinet/UDPClient.h>

static void LoggerFunc(unsigned int level, const char* file, unsigned int line,
                       const char* msg) {
  if (level == 20) {
    wpi::print(stderr, "DS: {}\n", msg);
    return;
  }

  std::string_view levelmsg;
  if (level >= 50) {
    levelmsg = "CRITICAL";
  } else if (level >= 40) {
    levelmsg = "ERROR";
  } else if (level >= 30) {
    levelmsg = "WARNING";
  } else {
    return;
  }
  wpi::print(stderr, "DS: {}: {} ({}:{})\n", levelmsg, msg, file, line);
}

static void generateEnabledDsPacket(wpi::SmallVectorImpl<uint8_t>& data,
                                    uint16_t sendCount) {
  data.clear();
  data.push_back(sendCount >> 8);
  data.push_back(sendCount);
  data.push_back(0x01);  // general data tag
  data.push_back(0x04);  // teleop enabled
  data.push_back(0x10);  // normal data request
  data.push_back(0x00);  // red 1 station
}

void MockDS::Start() {
  if (m_active) {
    return;
  }
  m_active = true;
  m_thread = std::thread([&]() {
    wpi::Logger logger(LoggerFunc);
    wpi::UDPClient client(logger);
    client.start();
    auto timeout_time = hal::fpga_clock::now();
    int initCount = 0;
    uint16_t sendCount = 0;
    wpi::SmallVector<uint8_t, 8> data;
    while (m_active) {
      // Keep 20ms intervals, and increase time to next interval
      auto current = hal::fpga_clock::now();
      while (timeout_time <= current) {
        timeout_time += std::chrono::milliseconds(20);
      }
      std::this_thread::sleep_until(timeout_time);
      generateEnabledDsPacket(data, sendCount++);
      // ~10 disabled packets are required to make the robot actually enable
      // 1 is definitely not enough.
      if (initCount < 10) {
        initCount++;
        data[3] = 0;
      }
      client.send(data, "127.0.0.1", 1110);
    }
    client.shutdown();
  });
}

void MockDS::Stop() {
  m_active = false;
  if (m_thread.joinable()) {
    m_thread.join();
  }
}
