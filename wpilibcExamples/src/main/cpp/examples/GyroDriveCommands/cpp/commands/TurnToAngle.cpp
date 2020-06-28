/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "commands/TurnToAngle.h"

#include <frc/controller/PIDController.h>

using namespace DriveConstants;

TurnToAngle::TurnToAngle(units::degree_t target, DriveSubsystem* drive)
    : CommandHelper(
          frc2::PIDController(kTurnP, kTurnI, kTurnD),
          // Close loop on heading
          [drive] { return drive->GetHeading().to<double>(); },
          // Set reference to target
          target.to<double>(),
          // Pipe output to turn robot
          [drive](double output) { drive->ArcadeDrive(0, output); },
          // Require the drive
          {drive}) {
  // Set the controller to be continuous (because it is an angle controller)
  m_controller.EnableContinuousInput(-180, 180);
  // Set the controller tolerance - the delta tolerance ensures the robot is
  // stationary at the setpoint before it is considered as having reached the
  // reference
  m_controller.SetTolerance(kTurnTolerance.to<double>(),
                            kTurnRateTolerance.to<double>());

  AddRequirements({drive});
}

bool TurnToAngle::IsFinished() { return GetController().AtSetpoint(); }
