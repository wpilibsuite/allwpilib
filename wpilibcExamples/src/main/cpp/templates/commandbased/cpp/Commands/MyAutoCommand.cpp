/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#include "Commands/MyAutoCommand.h"

#include "Robot.h"

MyAutoCommand::MyAutoCommand() {
  // Use Requires() here to declare subsystem dependencies
  Requires(&Robot::m_subsystem);
}

// Called just before this Command runs the first time
void MyAutoCommand::Initialize() {}

// Called repeatedly when this Command is scheduled to run
void MyAutoCommand::Execute() {}

// Make this return true when this Command no longer needs to run execute()
bool MyAutoCommand::IsFinished() { return false; }

// Called once after isFinished returns true
void MyAutoCommand::End() {}

// Called when another command which requires one or more of the same
// subsystems is scheduled to run
void MyAutoCommand::Interrupted() {}
