// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>
#include <utility>

#include "CommandTestBase.hpp"
#include "wpi/commands2/Commands.hpp"
#include "wpi/commands2/FunctionalCommand.hpp"
#include "wpi/commands2/InstantCommand.hpp"
#include "wpi/commands2/RunCommand.hpp"
#include "wpi/commands2/WaitUntilCommand.hpp"
#include "wpi/simulation/SimHooks.hpp"

using namespace wpi::cmd;
class CommandDecoratorTest : public CommandTestBase {};

TEST_CASE_METHOD(CommandDecoratorTest, "CommandDecoratorTest WithTimeout",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  wpi::sim::PauseTiming();

  auto command = Idle().WithTimeout(100_ms);

  scheduler.Schedule(command);
  scheduler.Run();

  CHECK(scheduler.IsScheduled(command));

  wpi::sim::StepTiming(150_ms);

  scheduler.Run();

  CHECK_FALSE(scheduler.IsScheduled(command));

  wpi::sim::ResumeTiming();
}

TEST_CASE_METHOD(CommandDecoratorTest, "CommandDecoratorTest Until",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  bool finish = false;

  auto command = Idle().Until([&finish] { return finish; });

  scheduler.Schedule(command);
  scheduler.Run();

  CHECK(scheduler.IsScheduled(command));

  finish = true;
  scheduler.Run();

  CHECK_FALSE(scheduler.IsScheduled(command));
}

TEST_CASE_METHOD(CommandDecoratorTest, "CommandDecoratorTest UntilOrder",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  bool firstHasRun = false;
  bool firstWasPolled = false;

  auto first = FunctionalCommand([] {}, [&firstHasRun] { firstHasRun = true; },
                                 [](bool interrupted) {},
                                 [&firstWasPolled] {
                                   firstWasPolled = true;
                                   return true;
                                 });
  auto command = std::move(first).Until([&firstHasRun, &firstWasPolled] {
    CHECK(firstHasRun);
    CHECK(firstWasPolled);
    return true;
  });

  scheduler.Schedule(command);
  scheduler.Run();

  CHECK(firstHasRun);
  CHECK(firstWasPolled);
}

TEST_CASE_METHOD(CommandDecoratorTest, "CommandDecoratorTest OnlyWhile",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  bool run = true;

  auto command = Idle().OnlyWhile([&run] { return run; });

  scheduler.Schedule(command);
  scheduler.Run();

  CHECK(scheduler.IsScheduled(command));

  run = false;
  scheduler.Run();

  CHECK_FALSE(scheduler.IsScheduled(command));
}

TEST_CASE_METHOD(CommandDecoratorTest, "CommandDecoratorTest OnlyWhileOrder",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  bool firstHasRun = false;
  bool firstWasPolled = false;

  auto first = FunctionalCommand([] {}, [&firstHasRun] { firstHasRun = true; },
                                 [](bool interrupted) {},
                                 [&firstWasPolled] {
                                   firstWasPolled = true;
                                   return true;
                                 });
  auto command = std::move(first).Until([&firstHasRun, &firstWasPolled] {
    CHECK(firstHasRun);
    CHECK(firstWasPolled);
    return false;
  });

  scheduler.Schedule(command);
  scheduler.Run();

  CHECK(firstHasRun);
  CHECK(firstWasPolled);
}

TEST_CASE_METHOD(CommandDecoratorTest, "CommandDecoratorTest IgnoringDisable",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  auto command = Idle().IgnoringDisable(true);

  SetDSEnabled(false);

  scheduler.Schedule(command);

  scheduler.Run();
  CHECK(scheduler.IsScheduled(command));
}

TEST_CASE_METHOD(CommandDecoratorTest, "CommandDecoratorTest BeforeStarting",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  bool finished = false;

  auto command = None().BeforeStarting([&finished] { finished = true; });

  scheduler.Schedule(command);

  CHECK(finished);

  scheduler.Run();

  CHECK(scheduler.IsScheduled(command));

  scheduler.Run();

  CHECK_FALSE(scheduler.IsScheduled(command));
}

TEST_CASE_METHOD(CommandDecoratorTest, "CommandDecoratorTest AndThenLambda",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  bool finished = false;

  auto command = None().AndThen([&finished] { finished = true; });

  scheduler.Schedule(command);

  CHECK_FALSE(finished);

  scheduler.Run();

  CHECK(finished);

  scheduler.Run();

  CHECK_FALSE(scheduler.IsScheduled(command));
}

TEST_CASE_METHOD(CommandDecoratorTest, "CommandDecoratorTest AndThen",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  bool finished = false;

  auto command1 = None();
  auto command2 = RunOnce([&finished] { finished = true; });
  auto group = std::move(command1).AndThen(std::move(command2));

  scheduler.Schedule(group);

  CHECK_FALSE(finished);

  scheduler.Run();

  CHECK(finished);

  scheduler.Run();

  CHECK_FALSE(scheduler.IsScheduled(group));
}

TEST_CASE_METHOD(CommandDecoratorTest, "CommandDecoratorTest DeadlineFor",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  bool finish = false;

  auto dictator = WaitUntil([&finish] { return finish; });
  auto endsAfter = Idle();

  auto group = std::move(dictator).DeadlineFor(std::move(endsAfter));

  scheduler.Schedule(group);
  scheduler.Run();

  CHECK(scheduler.IsScheduled(group));

  finish = true;
  scheduler.Run();

  CHECK_FALSE(scheduler.IsScheduled(group));
}

TEST_CASE_METHOD(CommandDecoratorTest, "CommandDecoratorTest WithDeadline",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  bool finish = false;

  auto dictator = WaitUntilCommand([&finish] { return finish; });
  auto endsAfter = WaitUntilCommand([] { return false; });

  auto group = std::move(endsAfter).WithDeadline(std::move(dictator).ToPtr());

  scheduler.Schedule(group);
  scheduler.Run();

  CHECK(scheduler.IsScheduled(group));

  finish = true;
  scheduler.Run();

  CHECK_FALSE(scheduler.IsScheduled(group));
}

TEST_CASE_METHOD(CommandDecoratorTest, "CommandDecoratorTest AlongWith",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  bool finish = false;

  auto command1 = WaitUntil([&finish] { return finish; });
  auto command2 = None();

  auto group = std::move(command1).AlongWith(std::move(command2));

  scheduler.Schedule(group);
  scheduler.Run();

  CHECK(scheduler.IsScheduled(group));

  finish = true;
  scheduler.Run();

  CHECK_FALSE(scheduler.IsScheduled(group));
}

TEST_CASE_METHOD(CommandDecoratorTest, "CommandDecoratorTest RaceWith",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  auto command1 = Idle();
  auto command2 = None();

  auto group = std::move(command1).RaceWith(std::move(command2));

  scheduler.Schedule(group);
  scheduler.Run();

  CHECK_FALSE(scheduler.IsScheduled(group));
}

TEST_CASE_METHOD(CommandDecoratorTest, "CommandDecoratorTest DeadlineForOrder",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  bool dictatorHasRun = false;
  bool dictatorWasPolled = false;

  auto dictator =
      FunctionalCommand([] {}, [&dictatorHasRun] { dictatorHasRun = true; },
                        [](bool interrupted) {},
                        [&dictatorWasPolled] {
                          dictatorWasPolled = true;
                          return true;
                        });
  auto other = wpi::cmd::Run([&dictatorHasRun, &dictatorWasPolled] {
    CHECK(dictatorHasRun);
    CHECK(dictatorWasPolled);
  });

  auto group = std::move(dictator).DeadlineFor(std::move(other));

  scheduler.Schedule(group);
  scheduler.Run();

  CHECK(dictatorHasRun);
  CHECK(dictatorWasPolled);
}

TEST_CASE_METHOD(CommandDecoratorTest, "CommandDecoratorTest WithDeadlineOrder",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  bool dictatorHasRun = false;
  bool dictatorWasPolled = false;

  auto dictator =
      FunctionalCommand([] {}, [&dictatorHasRun] { dictatorHasRun = true; },
                        [](bool interrupted) {},
                        [&dictatorWasPolled] {
                          dictatorWasPolled = true;
                          return true;
                        });
  auto other = RunCommand([&dictatorHasRun, &dictatorWasPolled] {
    CHECK(dictatorHasRun);
    CHECK(dictatorWasPolled);
  });

  auto group = std::move(other).WithDeadline(std::move(dictator).ToPtr());

  scheduler.Schedule(group);
  scheduler.Run();

  CHECK(dictatorHasRun);
  CHECK(dictatorWasPolled);
}

TEST_CASE_METHOD(CommandDecoratorTest, "CommandDecoratorTest AlongWithOrder",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  bool firstHasRun = false;
  bool firstWasPolled = false;

  auto command1 = FunctionalCommand(
      [] {}, [&firstHasRun] { firstHasRun = true; }, [](bool interrupted) {},
      [&firstWasPolled] {
        firstWasPolled = true;
        return true;
      });
  auto command2 = wpi::cmd::Run([&firstHasRun, &firstWasPolled] {
    CHECK(firstHasRun);
    CHECK(firstWasPolled);
  });

  auto group = std::move(command1).AlongWith(std::move(command2));

  scheduler.Schedule(group);
  scheduler.Run();

  CHECK(firstHasRun);
  CHECK(firstWasPolled);
}

TEST_CASE_METHOD(CommandDecoratorTest, "CommandDecoratorTest RaceWithOrder",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  bool firstHasRun = false;
  bool firstWasPolled = false;

  auto command1 = FunctionalCommand(
      [] {}, [&firstHasRun] { firstHasRun = true; }, [](bool interrupted) {},
      [&firstWasPolled] {
        firstWasPolled = true;
        return true;
      });
  auto command2 = wpi::cmd::Run([&firstHasRun, &firstWasPolled] {
    CHECK(firstHasRun);
    CHECK(firstWasPolled);
  });

  auto group = std::move(command1).RaceWith(std::move(command2));

  scheduler.Schedule(group);
  scheduler.Run();

  CHECK(firstHasRun);
  CHECK(firstWasPolled);
}

TEST_CASE_METHOD(CommandDecoratorTest, "CommandDecoratorTest Unless",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  bool hasRun = false;
  bool unlessCondition = true;

  auto command =
      RunOnce([&hasRun] { hasRun = true; }, {}).Unless([&unlessCondition] {
        return unlessCondition;
      });

  scheduler.Schedule(command);
  scheduler.Run();
  CHECK_FALSE(hasRun);

  unlessCondition = false;
  scheduler.Schedule(command);
  scheduler.Run();
  CHECK(hasRun);
}

TEST_CASE_METHOD(CommandDecoratorTest, "CommandDecoratorTest OnlyIf",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  bool hasRun = false;
  bool onlyIfCondition = false;

  auto command =
      RunOnce([&hasRun] { hasRun = true; }, {}).OnlyIf([&onlyIfCondition] {
        return onlyIfCondition;
      });

  scheduler.Schedule(command);
  scheduler.Run();
  CHECK_FALSE(hasRun);

  onlyIfCondition = true;
  scheduler.Schedule(command);
  scheduler.Run();
  CHECK(hasRun);
}

TEST_CASE_METHOD(CommandDecoratorTest, "CommandDecoratorTest FinallyDo",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();
  int first = 0;
  int second = 0;
  CommandPtr command = FunctionalCommand([] {}, [] {},
                                         [&first](bool interrupted) {
                                           if (!interrupted) {
                                             first++;
                                           }
                                         },
                                         [] { return true; })
                           .FinallyDo([&first, &second](bool interrupted) {
                             if (!interrupted) {
                               // to differentiate between "didn't run" and "ran
                               // before command's `end()`
                               second += 1 + first;
                             }
                           });

  scheduler.Schedule(command);
  CHECK(0 == first);
  CHECK(0 == second);
  scheduler.Run();
  CHECK(1 == first);
  // if `second == 0`, neither of the lambdas ran.
  // if `second == 1`, the second lambda ran before the first one
  CHECK(2 == second);
}

// handleInterruptTest() implicitly tests the interrupt=true branch of
// finallyDo()
TEST_CASE_METHOD(CommandDecoratorTest, "CommandDecoratorTest HandleInterrupt",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();
  int first = 0;
  int second = 0;
  CommandPtr command = FunctionalCommand([] {}, [] {},
                                         [&first](bool interrupted) {
                                           if (interrupted) {
                                             first++;
                                           }
                                         },
                                         [] { return false; })
                           .HandleInterrupt([&first, &second] {
                             // to differentiate between "didn't run" and "ran
                             // before command's `end()`
                             second += 1 + first;
                           });

  scheduler.Schedule(command);
  scheduler.Run();
  CHECK(0 == first);
  CHECK(0 == second);

  scheduler.Cancel(command);
  // if `second == 0`, neither of the lambdas ran.
  // if `second == 1`, the second lambda ran before the first one
  CHECK(2 == second);
}

TEST_CASE_METHOD(CommandDecoratorTest, "CommandDecoratorTest WithName",
                 "[commandsv2][command]") {
  auto command = None();
  std::string name{"Named"};
  auto named = std::move(command).WithName(name);
  CHECK(name == named.get()->GetName());
}
