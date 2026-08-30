// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <array>
#include <memory>
#include <string>
#include <vector>

#include "CommandTestBase.hpp"
#include "wpi/backend/NetworkTablesTunableBackend.hpp"
#include "wpi/commands2/FunctionalCommand.hpp"
#include "wpi/commands2/InstantCommand.hpp"
#include "wpi/commands2/RunCommand.hpp"
#include "wpi/nt/IntegerArrayTopic.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/tunables/MockTunableBackend.hpp"
#include "wpi/tunables/TunableConfig.hpp"
#include "wpi/tunables/TunableRegistry.hpp"
#include "wpi/tunables/Tunables.hpp"
#include "wpi/tunables/detail/TunableMember.hpp"

using namespace wpi::cmd;
class CommandScheduleTest : public CommandTestBase {};

class NetworkTablesTunableTestState {
 public:
  ~NetworkTablesTunableTestState() {
    wpi::tunables::TunableRegistry::Reset();
    wpi::nt::NetworkTableInstance::Destroy(inst);
  }

  wpi::nt::NetworkTableInstance inst{wpi::nt::NetworkTableInstance::Create()};
};

TEST_CASE_METHOD(CommandScheduleTest, "CommandScheduleTest InstantSchedule",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();
  MockCommand command;

  command.ExpectInitialize(1);
  command.ExpectExecute(1);
  command.ExpectEnd(false, 1);

  command.SetFinished(true);
  scheduler.Schedule(&command);
  CHECK(scheduler.IsScheduled(&command));
  scheduler.Run();
  CHECK_FALSE(scheduler.IsScheduled(&command));
}

TEST_CASE_METHOD(CommandScheduleTest,
                 "CommandScheduleTest SingleIterationSchedule",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();
  MockCommand command;

  command.ExpectInitialize(1);
  command.ExpectExecute(2);
  command.ExpectEnd(false, 1);

  scheduler.Schedule(&command);
  CHECK(scheduler.IsScheduled(&command));
  scheduler.Run();
  command.SetFinished(true);
  scheduler.Run();
  CHECK_FALSE(scheduler.IsScheduled(&command));
}

TEST_CASE_METHOD(CommandScheduleTest, "CommandScheduleTest MultiSchedule",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();
  MockCommand command1;
  MockCommand command2;
  MockCommand command3;

  command1.ExpectInitialize(1);
  command1.ExpectExecute(2);
  command1.ExpectEnd(false, 1);

  command2.ExpectInitialize(1);
  command2.ExpectExecute(3);
  command2.ExpectEnd(false, 1);

  command3.ExpectInitialize(1);
  command3.ExpectExecute(4);
  command3.ExpectEnd(false, 1);

  scheduler.Schedule(&command1);
  scheduler.Schedule(&command2);
  scheduler.Schedule(&command3);
  CHECK(scheduler.IsScheduled({&command1, &command2, &command3}));
  scheduler.Run();
  CHECK(scheduler.IsScheduled({&command1, &command2, &command3}));
  command1.SetFinished(true);
  scheduler.Run();
  CHECK(scheduler.IsScheduled({&command2, &command3}));
  CHECK_FALSE(scheduler.IsScheduled(&command1));
  command2.SetFinished(true);
  scheduler.Run();
  CHECK(scheduler.IsScheduled(&command3));
  CHECK_FALSE(scheduler.IsScheduled({&command1, &command2}));
  command3.SetFinished(true);
  scheduler.Run();
  CHECK_FALSE(scheduler.IsScheduled({&command1, &command2, &command3}));
}

TEST_CASE_METHOD(CommandScheduleTest, "CommandScheduleTest SchedulerCancel",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();
  MockCommand command;

  command.ExpectInitialize(1);
  command.ExpectExecute(1);
  command.ExpectEnd(false, 0);
  command.ExpectEnd(true, 1);

  scheduler.Schedule(&command);
  scheduler.Run();
  CHECK(scheduler.IsScheduled(&command));
  scheduler.Cancel(&command);
  scheduler.Run();
  CHECK_FALSE(scheduler.IsScheduled(&command));
}

TEST_CASE_METHOD(CommandScheduleTest,
                 "CommandScheduleTest CommandKnowsWhenItEnded",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  wpi::cmd::FunctionalCommand* commandPtr = nullptr;
  auto command = wpi::cmd::FunctionalCommand(
      [] {}, [] {},
      [&](auto isForced) { CHECK_FALSE(scheduler.IsScheduled(commandPtr)); },
      [] { return true; });
  commandPtr = &command;

  scheduler.Schedule(commandPtr);
  scheduler.Run();
  CHECK_FALSE(scheduler.IsScheduled(commandPtr));
}

TEST_CASE_METHOD(CommandScheduleTest,
                 "CommandScheduleTest ScheduleCommandInCommand",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();
  int counter = 0;
  wpi::cmd::InstantCommand commandToGetScheduled{[&counter] { counter++; }};

  auto command =
      wpi::cmd::RunCommand([&counter, &scheduler, &commandToGetScheduled] {
        scheduler.Schedule(&commandToGetScheduled);
        CHECK(counter == 1);
      });

  scheduler.Schedule(&command);
  scheduler.Run();
  CHECK(counter == 1);
  CHECK(scheduler.IsScheduled(&commandToGetScheduled));

  scheduler.Run();
  CHECK(counter == 1);
  CHECK_FALSE(scheduler.IsScheduled(&commandToGetScheduled));
}

TEST_CASE_METHOD(CommandScheduleTest, "CommandScheduleTest NotScheduledCancel",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();
  MockCommand command;

  CHECK_NOTHROW(scheduler.Cancel(&command));
}

TEST_CASE_METHOD(CommandScheduleTest, "CommandScheduleTest TunableCancel",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();
  auto backend = std::make_shared<wpi::tunables::MockTunableBackend>();
  wpi::tunables::TunableRegistry::RegisterBackend("", backend);
  wpi::tunables::Publish("Scheduler", scheduler);

  auto namesUid = backend->GetUid("/Scheduler/Names");
  CHECK(namesUid);
  auto namesInfo = wpi::tunables::TunableRegistry::GetTunable(*namesUid);
  REQUIRE(namesInfo);
  REQUIRE(namesInfo.config);
  CHECK_FALSE(namesInfo.config->isMutable);
  CHECK(namesInfo.config->polling ==
        wpi::tunables::TunableConfig::Polling::ALWAYS_GET);

  auto idsUid = backend->GetUid("/Scheduler/Ids");
  REQUIRE(idsUid);
  auto idsInfo = wpi::tunables::TunableRegistry::GetTunable(*idsUid);
  REQUIRE(idsInfo);
  REQUIRE(idsInfo.config);
  CHECK_FALSE(idsInfo.config->isMutable);
  CHECK(idsInfo.config->polling ==
        wpi::tunables::TunableConfig::Polling::ALWAYS_GET);

  MockCommand command;
  scheduler.Schedule(&command);
  scheduler.Run();
  CHECK(scheduler.IsScheduled(&command));
  wpi::tunables::TunableRegistry::Update();

  const auto& names = static_cast<wpi::tunables::detail::TunableMemberValueBase<
      std::vector<std::string>>*>(namesInfo.tunable)
                          ->Get(namesInfo.config->parent);
  REQUIRE(1U == names.size());
  CHECK(command.GetName() == names[0]);

  const auto& ids =
      static_cast<
          wpi::tunables::detail::TunableMemberValueBase<std::vector<int64_t>>*>(
          idsInfo.tunable)
          ->Get(idsInfo.config->parent);
  REQUIRE(1U == ids.size());

  uintptr_t ptrTmp = reinterpret_cast<uintptr_t>(&command);
  CHECK(static_cast<int64_t>(ptrTmp) == ids[0]);
  backend->SetInt64Vector(
      "/Scheduler/Cancel",
      std::span<const int64_t>{{static_cast<int64_t>(ptrTmp)}});
  wpi::tunables::TunableRegistry::Update();
  scheduler.Run();
  CHECK_FALSE(scheduler.IsScheduled(&command));
  wpi::tunables::TunableRegistry::Reset();
}

TEST_CASE_METHOD(CommandScheduleTest,
                 "CommandScheduleTest NetworkTablesTunableCancelClearsValue",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();
  NetworkTablesTunableTestState ntState;
  wpi::tunables::TunableRegistry::RegisterBackend(
      "", std::make_shared<wpi::backend::NetworkTablesTunableBackend>(
              ntState.inst, "/Tunables"));
  wpi::tunables::Publish("Scheduler", scheduler);

  auto valueSub =
      ntState.inst.GetIntegerArrayTopic("/Tunables/Scheduler/Cancel/value")
          .Subscribe({});
  CHECK((std::vector<int64_t>{}) == valueSub.Get());

  MockCommand command;
  scheduler.Schedule(&command);
  scheduler.Run();
  CHECK(scheduler.IsScheduled(&command));

  uintptr_t ptrTmp = reinterpret_cast<uintptr_t>(&command);
  int64_t commandId = static_cast<int64_t>(ptrTmp);
  std::array<int64_t, 1> cancelIds{commandId};
  ntState.inst.GetIntegerArrayTopic("/Tunables/Scheduler/Cancel/tune")
      .Publish()
      .Set(cancelIds);
  ntState.inst.Flush();

  wpi::tunables::TunableRegistry::Update();
  CHECK_FALSE(scheduler.IsScheduled(&command));

  wpi::tunables::TunableRegistry::Update();
  CHECK((std::vector<int64_t>{}) == valueSub.Get());
}
