/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef SIMPLE_ROBOT_H
#define SIMPLE_ROBOT_H

#include "RobotBase.h"

/**
 * @todo If this is going to last until release, it needs a better name.
 */
class SimpleRobot: public RobotBase
{
public:
	SimpleRobot();
	virtual ~SimpleRobot() {}
	virtual void RobotInit();
	virtual void Disabled();
	virtual void Autonomous();
    virtual void OperatorControl();
    virtual void Test();
	virtual void RobotMain();
	void StartCompetition();

private:
	bool m_robotMainOverridden;
};

#endif
