// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include "wpi/nt/NetworkTableInstance.hpp"

#include "../CommandTestBase.hpp"
#include "wpi/commands2/CommandScheduler.hpp"
#include "wpi/commands2/RunCommand.hpp"
#include "wpi/commands2/WaitUntilCommand.hpp"
#include "wpi/commands2/button/NetworkButton.hpp"

using namespace frc2;

class NetworkButtonTest : public CommandTestBase {
 public:
  NetworkButtonTest() {
    inst = nt::NetworkTableInstance::Create();
    inst.StartLocal();
  }

  ~NetworkButtonTest() override { nt::NetworkTableInstance::Destroy(inst); }

  nt::NetworkTableInstance inst;
};

TEST_F(NetworkButtonTest, SetNetworkButton) {
  auto& scheduler = CommandScheduler::GetInstance();
  auto pub = inst.GetTable("TestTable")->GetBooleanTopic("Test").Publish();
  bool finished = false;

  WaitUntilCommand command([&finished] { return finished; });

  NetworkButton(inst, "TestTable", "Test").OnTrue(&command);
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
  pub.Set(true);
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));
  finished = true;
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
}
