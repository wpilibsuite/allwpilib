// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>

#include "CommandTestBase.hpp"
#include "wpi/commands2/Commands.hpp"
#include "wpi/tunable/MockTunableBackend.hpp"
#include "wpi/tunable/TunableRegistry.hpp"
#include "wpi/tunable/Tunables.hpp"

using namespace wpi::cmd;

class CommandTunableButtonTest : public CommandTestBase {
 protected:
  int m_schedule;
  int m_cancel;
  std::shared_ptr<wpi::MockTunableBackend> m_backend;
  std::optional<CommandPtr> m_command;

  void SetUp() override {
    m_schedule = 0;
    m_cancel = 0;
    m_backend = std::make_shared<wpi::MockTunableBackend>();
    wpi::TunableRegistry::RegisterBackend("", m_backend);
    m_command = StartEnd([this] { m_schedule++; }, [this] { m_cancel++; });
    wpi::Tunables::Publish("command", *m_command->get());
  }

  void TearDown() override { wpi::TunableRegistry::Reset(); }
};

TEST_F(CommandTunableButtonTest, trueAndNotScheduledSchedules) {
  // Not scheduled and true -> scheduled
  GetScheduler().Run();
  EXPECT_FALSE(m_command->IsScheduled());
  EXPECT_EQ(0, m_schedule);
  EXPECT_EQ(0, m_cancel);

  m_backend->SetBool("/command/running", true);
  wpi::TunableRegistry::Update();
  GetScheduler().Run();
  EXPECT_TRUE(m_command->IsScheduled());
  EXPECT_EQ(1, m_schedule);
  EXPECT_EQ(0, m_cancel);
}

TEST_F(CommandTunableButtonTest, trueAndScheduledNoOp) {
  // Scheduled and true -> no-op
  wpi::cmd::CommandScheduler::GetInstance().Schedule(m_command.value());
  GetScheduler().Run();
  EXPECT_TRUE(m_command->IsScheduled());
  EXPECT_EQ(1, m_schedule);
  EXPECT_EQ(0, m_cancel);

  m_backend->SetBool("/command/running", true);
  wpi::TunableRegistry::Update();
  GetScheduler().Run();
  EXPECT_TRUE(m_command->IsScheduled());
  EXPECT_EQ(1, m_schedule);
  EXPECT_EQ(0, m_cancel);
}

TEST_F(CommandTunableButtonTest, falseAndNotScheduledNoOp) {
  // Not scheduled and false -> no-op
  GetScheduler().Run();
  EXPECT_FALSE(m_command->IsScheduled());
  EXPECT_EQ(0, m_schedule);
  EXPECT_EQ(0, m_cancel);

  m_backend->SetBool("/command/running", false);
  wpi::TunableRegistry::Update();
  GetScheduler().Run();
  EXPECT_FALSE(m_command->IsScheduled());
  EXPECT_EQ(0, m_schedule);
  EXPECT_EQ(0, m_cancel);
}

TEST_F(CommandTunableButtonTest, falseAndScheduledCancel) {
  // Scheduled and false -> cancel
  wpi::cmd::CommandScheduler::GetInstance().Schedule(m_command.value());
  GetScheduler().Run();
  EXPECT_TRUE(m_command->IsScheduled());
  EXPECT_EQ(1, m_schedule);
  EXPECT_EQ(0, m_cancel);

  m_backend->SetBool("/command/running", false);
  wpi::TunableRegistry::Update();
  GetScheduler().Run();
  EXPECT_FALSE(m_command->IsScheduled());
  EXPECT_EQ(1, m_schedule);
  EXPECT_EQ(1, m_cancel);
}
