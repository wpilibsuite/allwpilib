// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CommandTestBase.hpp"
#include "wpi/commands2/Commands.hpp"
#include "wpi/nt/BooleanTopic.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/smartdashboard/SmartDashboard.hpp"

using namespace wpi::cmd;

class CommandSendableButtonTest : public CommandTestBase {
 protected:
  int m_schedule;
  int m_cancel;
  wpi::nt::BooleanPublisher m_publish;
  std::optional<CommandPtr> m_command;

  void SetUp() override {
    m_schedule = 0;
    m_cancel = 0;
    m_command = cmd::StartEnd([this] { m_schedule++; }, [this] { m_cancel++; });
    m_publish = wpi::nt::NetworkTableInstance::GetDefault()
                    .GetBooleanTopic("/SmartDashboard/command/running")
                    .Publish();
    wpi::SmartDashboard::PutData("command", m_command->get());
    wpi::SmartDashboard::UpdateValues();
  }
};

TEST_F(CommandSendableButtonTest, trueAndNotScheduledSchedules) {
  // Not scheduled and true -> scheduled
  GetScheduler().Run();
  wpi::SmartDashboard::UpdateValues();
  EXPECT_FALSE(m_command->IsScheduled());
  EXPECT_EQ(0, m_schedule);
  EXPECT_EQ(0, m_cancel);

  m_publish.Set(true);
  wpi::SmartDashboard::UpdateValues();
  GetScheduler().Run();
  EXPECT_TRUE(m_command->IsScheduled());
  EXPECT_EQ(1, m_schedule);
  EXPECT_EQ(0, m_cancel);
}

TEST_F(CommandSendableButtonTest, trueAndScheduledNoOp) {
  // Scheduled and true -> no-op
  wpi::cmd::CommandScheduler::GetInstance().Schedule(m_command.value());
  GetScheduler().Run();
  wpi::SmartDashboard::UpdateValues();
  EXPECT_TRUE(m_command->IsScheduled());
  EXPECT_EQ(1, m_schedule);
  EXPECT_EQ(0, m_cancel);

  m_publish.Set(true);
  wpi::SmartDashboard::UpdateValues();
  GetScheduler().Run();
  EXPECT_TRUE(m_command->IsScheduled());
  EXPECT_EQ(1, m_schedule);
  EXPECT_EQ(0, m_cancel);
}

TEST_F(CommandSendableButtonTest, falseAndNotScheduledNoOp) {
  // Not scheduled and false -> no-op
  GetScheduler().Run();
  wpi::SmartDashboard::UpdateValues();
  EXPECT_FALSE(m_command->IsScheduled());
  EXPECT_EQ(0, m_schedule);
  EXPECT_EQ(0, m_cancel);

  m_publish.Set(false);
  wpi::SmartDashboard::UpdateValues();
  GetScheduler().Run();
  EXPECT_FALSE(m_command->IsScheduled());
  EXPECT_EQ(0, m_schedule);
  EXPECT_EQ(0, m_cancel);
}

TEST_F(CommandSendableButtonTest, falseAndScheduledCancel) {
  // Scheduled and false -> cancel
  wpi::cmd::CommandScheduler::GetInstance().Schedule(m_command.value());
  GetScheduler().Run();
  wpi::SmartDashboard::UpdateValues();
  EXPECT_TRUE(m_command->IsScheduled());
  EXPECT_EQ(1, m_schedule);
  EXPECT_EQ(0, m_cancel);

  m_publish.Set(false);
  wpi::SmartDashboard::UpdateValues();
  GetScheduler().Run();
  EXPECT_FALSE(m_command->IsScheduled());
  EXPECT_EQ(1, m_schedule);
  EXPECT_EQ(1, m_cancel);
}
