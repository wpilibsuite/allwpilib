// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>

#include <gtest/gtest.h>

#include "frc2/command/Command.h"
#include "frc2/command/InstantCommand.h"
#include "frc2/command/CommandConcepts.h"
#include "frc2/command/CommandPtr.h"
#include "frc2/command/Commands.h"

using namespace frc2;

TEST(CommandConceptsTest, OwnedCommand) {
  std::unique_ptr<Command> uniquePtr = std::make_unique<InstantCommand>([] {});
  InstantCommand instantCommand{[] {}};
  CommandPtr commandPtr = cmd::RunOnce([] {});
  [[maybe_unused]] CommandPtr test = cmd::Sequence(
      std::move(uniquePtr),
      std::move(instantCommand),
      std::move(commandPtr)
  );
}
