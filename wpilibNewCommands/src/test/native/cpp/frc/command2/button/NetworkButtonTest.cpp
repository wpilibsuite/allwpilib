// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <networktables/NetworkTableInstance.h>

#include "../CommandTestBase.h"
#include "frc/command2/CommandScheduler.h"
#include "frc/command2/RunCommand.h"
#include "frc/command2/WaitUntilCommand.h"
#include "frc/command2/button/NetworkButton.h"

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
  auto& scheduler = frc::CommandScheduler::GetInstance();
  auto pub = inst.GetTable("TestTable")->GetBooleanTopic("Test").Publish();
  bool finished = false;

  frc::WaitUntilCommand command([&finished] { return finished; });

  frc::NetworkButton(inst, "TestTable", "Test").OnTrue(&command);
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
  pub.Set(true);
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));
  finished = true;
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
}
