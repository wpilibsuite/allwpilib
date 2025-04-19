// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc2/command/Commands.h>

#include <frc/smartdashboard/SmartDashboard.h>
#include <networktables/BooleanTopic.h>
#include <networktables/NetworkTableInstance.h>

#include "CommandTestBase.h"

using namespace frc2;

class CommandSendableButtonTest : public CommandTestBase {
 protected:
  int m_schedule;
  int m_cancel;
  nt::BooleanPublisher m_publish;
  std::optional<CommandPtr> m_command;

  void SetUp() override {
    m_schedule = 0;
    m_cancel = 0;
    m_command = cmd::StartEnd([this] { m_schedule++; }, [this] { m_cancel++; });
    m_publish = nt::NetworkTableInstance::GetDefault()
                    .GetBooleanTopic("/SmartDashboard/command/running")
                    .Publish();
    frc::SmartDashboard::PutData("command", m_command->get());
    frc::SmartDashboard::UpdateValues();
  }
};

TEST_F(CommandSendableButtonTest, trueAndNotScheduledSchedules) {
  // Not scheduled and true -> scheduled
  GetScheduler().Run();
  frc::SmartDashboard::UpdateValues();
  EXPECT_FALSE(m_command->IsScheduled());
  EXPECT_EQ(0, m_schedule);
  EXPECT_EQ(0, m_cancel);

  m_publish.Set(true);
  frc::SmartDashboard::UpdateValues();
  GetScheduler().Run();
  EXPECT_TRUE(m_command->IsScheduled());
  EXPECT_EQ(1, m_schedule);
  EXPECT_EQ(0, m_cancel);
}

TEST_F(CommandSendableButtonTest, trueAndScheduledNoOp) {
  // Scheduled and true -> no-op
  frc2::CommandScheduler::GetInstance().Schedule(m_command.value());
  GetScheduler().Run();
  frc::SmartDashboard::UpdateValues();
  EXPECT_TRUE(m_command->IsScheduled());
  EXPECT_EQ(1, m_schedule);
  EXPECT_EQ(0, m_cancel);

  m_publish.Set(true);
  frc::SmartDashboard::UpdateValues();
  GetScheduler().Run();
  EXPECT_TRUE(m_command->IsScheduled());
  EXPECT_EQ(1, m_schedule);
  EXPECT_EQ(0, m_cancel);
}

TEST_F(CommandSendableButtonTest, falseAndNotScheduledNoOp) {
  // Not scheduled and false -> no-op
  GetScheduler().Run();
  frc::SmartDashboard::UpdateValues();
  EXPECT_FALSE(m_command->IsScheduled());
  EXPECT_EQ(0, m_schedule);
  EXPECT_EQ(0, m_cancel);

  m_publish.Set(false);
  frc::SmartDashboard::UpdateValues();
  GetScheduler().Run();
  EXPECT_FALSE(m_command->IsScheduled());
  EXPECT_EQ(0, m_schedule);
  EXPECT_EQ(0, m_cancel);
}

TEST_F(CommandSendableButtonTest, falseAndScheduledCancel) {
  // Scheduled and false -> cancel
  frc2::CommandScheduler::GetInstance().Schedule(m_command.value());
  GetScheduler().Run();
  frc::SmartDashboard::UpdateValues();
  EXPECT_TRUE(m_command->IsScheduled());
  EXPECT_EQ(1, m_schedule);
  EXPECT_EQ(0, m_cancel);

  m_publish.Set(false);
  frc::SmartDashboard::UpdateValues();
  GetScheduler().Run();
  EXPECT_FALSE(m_command->IsScheduled());
  EXPECT_EQ(1, m_schedule);
  EXPECT_EQ(1, m_cancel);
}
