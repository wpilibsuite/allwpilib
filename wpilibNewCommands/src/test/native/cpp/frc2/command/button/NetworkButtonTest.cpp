// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <networktables/NetworkTableInstance.h>

#include "../CommandTestBase.h"
#include "frc2/command/CommandScheduler.h"
#include "frc2/command/RunCommand.h"
#include "frc2/command/WaitUntilCommand.h"
#include "frc2/command/button/NetworkButton.h"
#include "gtest/gtest.h"

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
