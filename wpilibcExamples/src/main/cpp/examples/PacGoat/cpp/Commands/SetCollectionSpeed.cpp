/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#include "Commands/SetCollectionSpeed.h"

#include "Robot.h"

SetCollectionSpeed::SetCollectionSpeed(double speed) {
  Requires(&Robot::collector);
  m_speed = speed;
}

// Called just before this Command runs the first time
void SetCollectionSpeed::Initialize() { Robot::collector.SetSpeed(m_speed); }
