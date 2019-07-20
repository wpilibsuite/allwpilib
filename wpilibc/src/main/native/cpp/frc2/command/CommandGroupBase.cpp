/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc2/command/CommandGroupBase.h"

#include <set>

#include "frc/WPIErrors.h"
#include "frc2/command/ParallelCommandGroup.h"
#include "frc2/command/ParallelDeadlineGroup.h"
#include "frc2/command/ParallelRaceGroup.h"
#include "frc2/command/SequentialCommandGroup.h"

using namespace frc2;
template <typename TMap, typename TKey>
static bool ContainsKey(const TMap& map, TKey keyToCheck) {
  return map.find(keyToCheck) != map.end();
}
bool CommandGroupBase::RequireUngrouped(Command& command) {
  if (command.IsGrouped()) {
    wpi_setGlobalWPIErrorWithContext(
        CommandIllegalUse,
        "Commands cannot be added to more than one CommandGroup");
    return false;
  } else {
    return true;
  }
}

bool CommandGroupBase::RequireUngrouped(
    wpi::ArrayRef<std::unique_ptr<Command>> commands) {
  bool allUngrouped = true;
  for (auto&& command : commands) {
    allUngrouped &= !command.get()->IsGrouped();
  }
  if (!allUngrouped) {
    wpi_setGlobalWPIErrorWithContext(
        CommandIllegalUse,
        "Commands cannot be added to more than one CommandGroup");
  }
  return allUngrouped;
}

bool CommandGroupBase::RequireUngrouped(
    std::initializer_list<Command*> commands) {
  bool allUngrouped = true;
  for (auto&& command : commands) {
    allUngrouped &= !command->IsGrouped();
  }
  if (!allUngrouped) {
    wpi_setGlobalWPIErrorWithContext(
        CommandIllegalUse,
        "Commands cannot be added to more than one CommandGroup");
  }
  return allUngrouped;
}
