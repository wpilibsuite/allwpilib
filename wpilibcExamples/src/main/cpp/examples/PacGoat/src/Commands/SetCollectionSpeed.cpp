/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SetCollectionSpeed.h"

#include "../Robot.h"

SetCollectionSpeed::SetCollectionSpeed(double speed) {
	Requires(&Robot::collector);
	m_speed = speed;
}

// Called just before this Command runs the first time
void SetCollectionSpeed::Initialize() {
	Robot::collector.SetSpeed(m_speed);
}
