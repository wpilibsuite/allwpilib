// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>

#include "CommandTestBase.hpp"
#include "wpi/commands2/Commands.hpp"
#include "wpi/tunable/MockTunableBackend.hpp"
#include "wpi/tunable/TunableConfig.hpp"
#include "wpi/tunable/TunableRegistry.hpp"
#include "wpi/tunable/Tunables.hpp"

using namespace wpi::cmd;

class CommandTunableButtonTest : public CommandTestBase {
 protected:
  CommandTunableButtonTest() {
    m_schedule = 0;
    m_cancel = 0;
    m_backend = std::make_shared<wpi::MockTunableBackend>();
    wpi::TunableRegistry::RegisterBackend("", m_backend);
    m_command = StartEnd([this] { m_schedule++; }, [this] { m_cancel++; });
    wpi::Tunables::Publish("command", *m_command->get());
  }

  ~CommandTunableButtonTest() override { wpi::TunableRegistry::Reset(); }

  int m_schedule;
  int m_cancel;
  std::shared_ptr<wpi::MockTunableBackend> m_backend;
  std::optional<CommandPtr> m_command;
};

TEST_CASE_METHOD(CommandTunableButtonTest,
                 "CommandTunableButtonTest trueAndNotScheduledSchedules",
                 "[commandsv2][command]") {
  // Not scheduled and true -> scheduled
  GetScheduler().Run();
  CHECK_FALSE(m_command->IsScheduled());
  CHECK(0 == m_schedule);
  CHECK(0 == m_cancel);

  m_backend->SetBool("/command/running", true);
  wpi::TunableRegistry::Update();
  GetScheduler().Run();
  CHECK(m_command->IsScheduled());
  CHECK(1 == m_schedule);
  CHECK(0 == m_cancel);
}

TEST_CASE_METHOD(
    CommandTunableButtonTest,
    "CommandTunableButtonTest runningTunableAlwaysGetsSchedulerState",
    "[commandsv2][command]") {
  auto uid = m_backend->GetUid("/command/running");
  REQUIRE(uid);

  auto info = wpi::TunableRegistry::GetTunable(*uid);
  REQUIRE(info);
  REQUIRE(info.config != nullptr);
  CHECK(info.config->alwaysGet);
}

TEST_CASE_METHOD(CommandTunableButtonTest,
                 "CommandTunableButtonTest trueAndScheduledNoOp",
                 "[commandsv2][command]") {
  // Scheduled and true -> no-op
  wpi::cmd::CommandScheduler::GetInstance().Schedule(m_command.value());
  GetScheduler().Run();
  CHECK(m_command->IsScheduled());
  CHECK(1 == m_schedule);
  CHECK(0 == m_cancel);

  m_backend->SetBool("/command/running", true);
  wpi::TunableRegistry::Update();
  GetScheduler().Run();
  CHECK(m_command->IsScheduled());
  CHECK(1 == m_schedule);
  CHECK(0 == m_cancel);
}

TEST_CASE_METHOD(CommandTunableButtonTest,
                 "CommandTunableButtonTest falseAndNotScheduledNoOp",
                 "[commandsv2][command]") {
  // Not scheduled and false -> no-op
  GetScheduler().Run();
  CHECK_FALSE(m_command->IsScheduled());
  CHECK(0 == m_schedule);
  CHECK(0 == m_cancel);

  m_backend->SetBool("/command/running", false);
  wpi::TunableRegistry::Update();
  GetScheduler().Run();
  CHECK_FALSE(m_command->IsScheduled());
  CHECK(0 == m_schedule);
  CHECK(0 == m_cancel);
}

TEST_CASE_METHOD(CommandTunableButtonTest,
                 "CommandTunableButtonTest falseAndScheduledCancel",
                 "[commandsv2][command]") {
  // Scheduled and false -> cancel
  wpi::cmd::CommandScheduler::GetInstance().Schedule(m_command.value());
  GetScheduler().Run();
  CHECK(m_command->IsScheduled());
  CHECK(1 == m_schedule);
  CHECK(0 == m_cancel);

  m_backend->SetBool("/command/running", false);
  wpi::TunableRegistry::Update();
  GetScheduler().Run();
  CHECK_FALSE(m_command->IsScheduled());
  CHECK(1 == m_schedule);
  CHECK(1 == m_cancel);
}
