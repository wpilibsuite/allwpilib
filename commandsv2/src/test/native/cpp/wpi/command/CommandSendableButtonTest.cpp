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
  CommandSendableButtonTest() {
    m_schedule = 0;
    m_cancel = 0;
    m_command = StartEnd([this] { m_schedule++; }, [this] { m_cancel++; });
    m_publish = wpi::nt::NetworkTableInstance::GetDefault()
                    .GetBooleanTopic("/SmartDashboard/command/running")
                    .Publish();
    wpi::SmartDashboard::PutData("command", m_command->get());
    wpi::SmartDashboard::UpdateValues();
  }

  int m_schedule;
  int m_cancel;
  wpi::nt::BooleanPublisher m_publish;
  std::optional<CommandPtr> m_command;
};

TEST_CASE_METHOD(CommandSendableButtonTest,
                 "CommandSendableButtonTest trueAndNotScheduledSchedules",
                 "[commandsv2][command]") {
  // Not scheduled and true -> scheduled
  GetScheduler().Run();
  wpi::SmartDashboard::UpdateValues();
  CHECK_FALSE(m_command->IsScheduled());
  CHECK(0 == m_schedule);
  CHECK(0 == m_cancel);

  m_publish.Set(true);
  wpi::SmartDashboard::UpdateValues();
  GetScheduler().Run();
  CHECK(m_command->IsScheduled());
  CHECK(1 == m_schedule);
  CHECK(0 == m_cancel);
}

TEST_CASE_METHOD(CommandSendableButtonTest,
                 "CommandSendableButtonTest trueAndScheduledNoOp",
                 "[commandsv2][command]") {
  // Scheduled and true -> no-op
  wpi::cmd::CommandScheduler::GetInstance().Schedule(m_command.value());
  GetScheduler().Run();
  wpi::SmartDashboard::UpdateValues();
  CHECK(m_command->IsScheduled());
  CHECK(1 == m_schedule);
  CHECK(0 == m_cancel);

  m_publish.Set(true);
  wpi::SmartDashboard::UpdateValues();
  GetScheduler().Run();
  CHECK(m_command->IsScheduled());
  CHECK(1 == m_schedule);
  CHECK(0 == m_cancel);
}

TEST_CASE_METHOD(CommandSendableButtonTest,
                 "CommandSendableButtonTest falseAndNotScheduledNoOp",
                 "[commandsv2][command]") {
  // Not scheduled and false -> no-op
  GetScheduler().Run();
  wpi::SmartDashboard::UpdateValues();
  CHECK_FALSE(m_command->IsScheduled());
  CHECK(0 == m_schedule);
  CHECK(0 == m_cancel);

  m_publish.Set(false);
  wpi::SmartDashboard::UpdateValues();
  GetScheduler().Run();
  CHECK_FALSE(m_command->IsScheduled());
  CHECK(0 == m_schedule);
  CHECK(0 == m_cancel);
}

TEST_CASE_METHOD(CommandSendableButtonTest,
                 "CommandSendableButtonTest falseAndScheduledCancel",
                 "[commandsv2][command]") {
  // Scheduled and false -> cancel
  wpi::cmd::CommandScheduler::GetInstance().Schedule(m_command.value());
  GetScheduler().Run();
  wpi::SmartDashboard::UpdateValues();
  CHECK(m_command->IsScheduled());
  CHECK(1 == m_schedule);
  CHECK(0 == m_cancel);

  m_publish.Set(false);
  wpi::SmartDashboard::UpdateValues();
  GetScheduler().Run();
  CHECK_FALSE(m_command->IsScheduled());
  CHECK(1 == m_schedule);
  CHECK(1 == m_cancel);
}
