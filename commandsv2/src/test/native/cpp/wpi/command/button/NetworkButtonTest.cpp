// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/button/NetworkButton.hpp"

#include <catch2/catch_test_macros.hpp>

#include "../CommandTestBase.hpp"
#include "wpi/commands2/CommandScheduler.hpp"
#include "wpi/commands2/WaitUntilCommand.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"

using namespace wpi::cmd;

class NetworkButtonTest : public CommandTestBase {
 public:
  NetworkButtonTest() {
    inst = wpi::nt::NetworkTableInstance::Create();
    inst.StartLocal();
  }

  ~NetworkButtonTest() override {
    wpi::nt::NetworkTableInstance::Destroy(inst);
  }

  wpi::nt::NetworkTableInstance inst;
};

TEST_CASE_METHOD(NetworkButtonTest, "NetworkButtonTest SetNetworkButton",
                 "[commandsv2][command]") {
  auto& scheduler = CommandScheduler::GetInstance();
  auto pub = inst.GetTable("TestTable")->GetBooleanTopic("Test").Publish();
  bool finished = false;

  WaitUntilCommand command([&finished] { return finished; });

  NetworkButton(inst, "TestTable", "Test").OnTrue(&command);
  scheduler.Run();
  CHECK_FALSE(scheduler.IsScheduled(&command));
  pub.Set(true);
  scheduler.Run();
  CHECK(scheduler.IsScheduled(&command));
  finished = true;
  scheduler.Run();
  CHECK_FALSE(scheduler.IsScheduled(&command));
}
