#pragma once

#include "subsystems/DriveSubsystem.h"

#include <frc2/command/TrapezoidProfileCommand.h>
#include <frc2/command/CommandHelper.h>

class DriveDistanceProfiled
    : public frc2::CommandHelper<frc2::TrapezoidProfileCommand<units::meters>,
    DriveDistanceProfiled> {
 public:
  DriveDistanceProfiled(units::meter_t distance, DriveSubsystem drive);
};
