/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

class RobotStateInterface
{
public:
	virtual ~RobotStateInterface() {};
    virtual bool IsDisabled() = 0;
	virtual bool IsEnabled() = 0;
	virtual bool IsOperatorControl() = 0;
	virtual bool IsAutonomous() = 0;
	virtual bool IsTest() = 0;
};

class RobotState
{
private:
	static RobotStateInterface* impl;
	
public:
	static void SetImplementation(RobotStateInterface* i);
    static bool IsDisabled();
	static bool IsEnabled();
	static bool IsOperatorControl();
	static bool IsAutonomous();
	static bool IsTest();
};


