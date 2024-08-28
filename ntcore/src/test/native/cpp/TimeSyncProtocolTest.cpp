// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <net/TimeSyncClientServer.h>

#include <gtest/gtest.h>

TEST(TimeSyncProtocolTest, TestClient) {
  using namespace wpi;
  using namespace std::chrono_literals;

  static auto server_bogus_offset = -nt::Now();
  TimeSyncServer server{5810, []() { return nt::Now() + server_bogus_offset; }};
  TimeSyncClient client{"127.0.0.1", 5810, 1s};

  server.Start();
  std::this_thread::sleep_for(0.5s);
  client.Start();

  for (int i = 0; i < 10; i++) {
    auto off = client.GetOffset();
    wpi::println("Unit Test: current offset = {} uS, error = {} uS", off,
                 off - static_cast<int64_t>(server_bogus_offset));
    std::this_thread::sleep_for(1s);
  }

  server.Stop();
  client.Stop();
}
