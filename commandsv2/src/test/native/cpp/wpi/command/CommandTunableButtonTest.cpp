// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>

#include "CommandTestBase.hpp"
#include "wpi/backend/NetworkTablesTunableBackend.hpp"
#include "wpi/commands2/Commands.hpp"
#include "wpi/commands2/FunctionalCommand.hpp"
#include "wpi/nt/BooleanTopic.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/StringTopic.hpp"
#include "wpi/tunables/MockTunableBackend.hpp"
#include "wpi/tunables/TunableConfig.hpp"
#include "wpi/tunables/TunableRegistry.hpp"
#include "wpi/tunables/Tunables.hpp"

using namespace wpi::cmd;

class CommandTunableButtonTest : public CommandTestBase {
 protected:
  CommandTunableButtonTest() {
    m_schedule = 0;
    m_cancel = 0;
    m_backend = std::make_shared<wpi::tunables::MockTunableBackend>();
    wpi::tunables::TunableRegistry::RegisterBackend("", m_backend);
    m_command = StartEnd([this] { m_schedule++; }, [this] { m_cancel++; });
    wpi::tunables::Publish("command", *m_command->get());
  }

  ~CommandTunableButtonTest() override {
    wpi::tunables::TunableRegistry::Reset();
  }

  int m_schedule;
  int m_cancel;
  std::shared_ptr<wpi::tunables::MockTunableBackend> m_backend;
  std::optional<CommandPtr> m_command;
};

class NetworkTablesTunableTestState {
 public:
  ~NetworkTablesTunableTestState() {
    wpi::tunables::TunableRegistry::Reset();
    wpi::nt::NetworkTableInstance::Destroy(inst);
  }

  wpi::nt::NetworkTableInstance inst{wpi::nt::NetworkTableInstance::Create()};
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
  wpi::tunables::TunableRegistry::Update();
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

  auto info = wpi::tunables::TunableRegistry::GetTunable(*uid);
  REQUIRE(info);
  REQUIRE(info.config != nullptr);
  CHECK(info.config->polling ==
        wpi::tunables::TunableConfig::Polling::ALWAYS_GET);
}

TEST_CASE_METHOD(CommandTunableButtonTest,
                 "CommandTunableButtonTest nameTunablePublishesCommandName",
                 "[commandsv2][command]") {
  m_command->get()->SetName("Renamed Command");
  wpi::tunables::TunableRegistry::Update();

  auto uid = m_backend->GetUid("/command/name");
  REQUIRE(uid);

  auto info = wpi::tunables::TunableRegistry::GetTunable(*uid);
  REQUIRE(info);
  REQUIRE(info.config != nullptr);
  CHECK_FALSE(info.config->isMutable);
  CHECK(info.config->polling ==
        wpi::tunables::TunableConfig::Polling::GET_ON_CHANGE);
  CHECK("Renamed Command" == m_backend->GetString("/command/name"));
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
  wpi::tunables::TunableRegistry::Update();
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
  wpi::tunables::TunableRegistry::Update();
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
  wpi::tunables::TunableRegistry::Update();
  GetScheduler().Run();
  CHECK_FALSE(m_command->IsScheduled());
  CHECK(1 == m_schedule);
  CHECK(1 == m_cancel);
}

TEST_CASE_METHOD(
    CommandTestBase,
    "CommandTunableButtonTest NetworkTablesLifecycleMatchesGlassRunCancel",
    "[commandsv2][command]") {
  int initializeCount = 0;
  int executeCount = 0;
  int interruptedEndCount = 0;
  int finishedEndCount = 0;
  FunctionalCommand command{[&] { ++initializeCount; }, [&] { ++executeCount; },
                            [&](bool interrupted) {
                              if (interrupted) {
                                ++interruptedEndCount;
                              } else {
                                ++finishedEndCount;
                              }
                            },
                            [] { return false; }};
  command.SetName("Glass Label");
  NetworkTablesTunableTestState ntState;
  wpi::tunables::TunableRegistry::RegisterBackend(
      "", std::make_shared<wpi::backend::NetworkTablesTunableBackend>(
              ntState.inst, "/Tunables"));
  wpi::tunables::Publish("command", command);

  auto name =
      ntState.inst.GetStringTopic("/Tunables/command/name").Subscribe("");
  CHECK(false ==
        ntState.inst.GetTopic("/Tunables/command/name").GetProperty("mutable"));
  CHECK("Glass Label" == name.Get());

  auto running =
      ntState.inst.GetBooleanTopic("/Tunables/command/running").GetEntry(false);
  CHECK(true == ntState.inst.GetTopic("/Tunables/command/running")
                    .GetProperty("mutable"));
  CHECK_FALSE(running.Get());
  CHECK_FALSE(command.IsScheduled());

  running.Set(true);
  ntState.inst.Flush();
  wpi::tunables::TunableRegistry::Update();

  CHECK(command.IsScheduled());
  CHECK(running.Get());
  CHECK(1 == initializeCount);
  CHECK(0 == executeCount);
  CHECK(0 == interruptedEndCount);
  CHECK(0 == finishedEndCount);

  CommandScheduler::GetInstance().Run();
  CHECK(1 == executeCount);
  wpi::tunables::TunableRegistry::Update();
  CHECK(running.Get());

  command.SetName("Updated Label");
  wpi::tunables::TunableRegistry::Update();
  CHECK("Updated Label" == name.Get());

  running.Set(false);
  ntState.inst.Flush();
  wpi::tunables::TunableRegistry::Update();

  CHECK_FALSE(command.IsScheduled());
  CHECK_FALSE(running.Get());
  CHECK(1 == initializeCount);
  CHECK(1 == executeCount);
  CHECK(1 == interruptedEndCount);
  CHECK(0 == finishedEndCount);

  CommandScheduler::GetInstance().Run();
  CHECK(1 == executeCount);
  wpi::tunables::TunableRegistry::Update();
  CHECK_FALSE(running.Get());
}
