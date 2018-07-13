/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#include "OI.h"

#include <SmartDashboard/SmartDashboard.h>

#include "Commands/Autonomous.h"
#include "Commands/CloseClaw.h"
#include "Commands/OpenClaw.h"
#include "Commands/Pickup.h"
#include "Commands/Place.h"
#include "Commands/PrepareToPickup.h"
#include "Commands/SetElevatorSetpoint.h"

OI::OI() {
  frc::SmartDashboard::PutData("Open Claw", new OpenClaw());
  frc::SmartDashboard::PutData("Close Claw", new CloseClaw());

  // Connect the buttons to commands
  m_dUp.WhenPressed(new SetElevatorSetpoint(0.2));
  m_dDown.WhenPressed(new SetElevatorSetpoint(-0.2));
  m_dRight.WhenPressed(new CloseClaw());
  m_dLeft.WhenPressed(new OpenClaw());

  m_r1.WhenPressed(new PrepareToPickup());
  m_r2.WhenPressed(new Pickup());
  m_l1.WhenPressed(new Place());
  m_l2.WhenPressed(new Autonomous());
}

frc::Joystick& OI::GetJoystick() { return m_joy; }
