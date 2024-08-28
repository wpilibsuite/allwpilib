// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <net/TimeSyncClientServer.h>

#include <gtest/gtest.h>

TEST(TimeSyncProtocolTest, TestClient) {
  using namespace wpi;
  using namespace std::chrono_literals;

  TimeSyncClient client{"127.0.0.1", 5810, 1s};

  client.Start();

  for (int i = 0; i < 10; i++) {
    wpi::println("Got offset {}", client.GetOffset());
    std::this_thread::sleep_for(1s);
  }
}
