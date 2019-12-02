/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "commands/TurnToAngleProfiled.h"

#include <frc/controller/ProfiledPIDController.h>

using namespace DriveConstants;

TurnToAngleProfiled::TurnToAngleProfiled(units::degree_t target,
                                         DriveSubsystem* drive)
    : CommandHelper(
          frc::ProfiledPIDController<units::radians>(
              kTurnP, kTurnI, kTurnD, {kMaxTurnRate, kMaxTurnAcceleration}),
          // Close loop on heading
          [drive] { return drive->GetHeading(); },
          // Set reference to target
          target,
          // Pipe output to turn robot
          [drive](double output, auto setpointState) {
            drive->ArcadeDrive(0, output);
          },
          // Require the drive
          {drive}) {
  // Set the controller to be continuous (because it is an angle controller)
  GetController().EnableContinuousInput(-180_deg, 180_deg);
  // Set the controller tolerance - the delta tolerance ensures the robot is
  // stationary at the setpoint before it is considered as having reached the
  // reference
  GetController().SetTolerance(kTurnTolerance, kTurnRateTolerance);

  AddRequirements({drive});
}

bool TurnToAngleProfiled::IsFinished() { return GetController().AtGoal(); }
