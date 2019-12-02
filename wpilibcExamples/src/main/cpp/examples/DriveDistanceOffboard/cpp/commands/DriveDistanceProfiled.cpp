#include "commands/DriveDistanceProfiled.h"
#include "Constants.h"

using namespace DriveConstants;

DriveDistanceProfiled::DriveDistanceProfiled(units::meter_t distance, DriveSubsystem drive)
    : CommandHelper(
      frc::TrapezoidProfile<units::meters>(
        // Limit the max acceleration and velocity
        {kMaxSpeed, kMaxAcceleration},
        // End at desired position in meters; implicitly starts at 0
        {distance, 0_mps}),
      // Pipe the profile state to the drive
      [&drive](auto setpointState)
          {drive.SetDriveStates(setpointState, setpointState);},
      // Require the drive
      {&drive}
    ) {
  // Reset drive encoders since we're starting at 0
  drive.ResetEncoders();
}