// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/event/NetworkBooleanEvent.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "wpi/event/EventLoop.hpp"
#include "wpi/nt/BooleanTopic.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"

using namespace wpi;

class NetworkBooleanEventTest {
 public:
  NetworkBooleanEventTest() {
    m_inst = wpi::nt::NetworkTableInstance::Create();
    m_inst.StartLocal();
  }

  ~NetworkBooleanEventTest() { wpi::nt::NetworkTableInstance::Destroy(m_inst); }

  wpi::nt::NetworkTableInstance m_inst;
};

TEST_CASE_METHOD(NetworkBooleanEventTest, "NetworkBooleanEventTest Set",
                 "[wpilibc][event]") {
  EventLoop loop;
  int counter = 0;

  auto pub = m_inst.GetTable("TestTable")->GetBooleanTopic("Test").Publish();

  NetworkBooleanEvent(&loop, m_inst, "TestTable", "Test").IfHigh([&] {
    ++counter;
  });
  pub.Set(false);
  loop.Poll();
  CHECK(0 == counter);
  pub.Set(true);
  loop.Poll();
  CHECK(1 == counter);
  pub.Set(false);
  loop.Poll();
  CHECK(1 == counter);
}
