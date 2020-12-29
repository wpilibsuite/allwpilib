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
  void SetUp() override { nt::NetworkTableInstance::GetDefault().StartLocal(); }

  void TearDown() override {
    nt::NetworkTableInstance::GetDefault().DeleteAllEntries();
    nt::NetworkTableInstance::GetDefault().StopLocal();
  }
};

TEST_F(NetworkButtonTest, SetNetworkButtonTest) {
  auto& scheduler = CommandScheduler::GetInstance();
  auto entry = nt::NetworkTableInstance::GetDefault()
                   .GetTable("TestTable")
                   ->GetEntry("Test");
  bool finished = false;

  WaitUntilCommand command([&finished] { return finished; });

  NetworkButton("TestTable", "Test").WhenActive(&command);
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
  entry.SetBoolean(true);
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));
  finished = true;
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
}
