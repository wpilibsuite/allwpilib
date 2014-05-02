/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "Base.h"
#include "Task.h"

class DriverStation;

#ifdef __vxworks
#define START_ROBOT_CLASS(_ClassName_) \
	RobotBase *FRC_userClassFactory() \
	{ \
	    cout << "Instantiating " #_ClassName_ "...\n";\
		RobotBase* rb= new _ClassName_();\
	    cout << "...Instantiated " #_ClassName_ "\n";\
		return rb;\
	} \
	extern "C" { \
		int32_t FRC_UserProgram_StartupLibraryInit() \
		{ \
			RobotBase::startRobotTask((FUNCPTR)FRC_userClassFactory); \
			return 0; \
		} \
	}
#else
#define START_ROBOT_CLASS(_ClassName_) \
	int main() \
	{ \
		if (!HALInitialize()){std::cerr<<"FATAL ERROR: HAL could not be initialized"<<std::endl;return -1;}	\
		(new _ClassName_())->StartCompetition(); \
		return 0; \
	}
#endif

/**
 * Implement a Robot Program framework.
 * The RobotBase class is intended to be subclassed by a user creating a robot program.
 * Overridden Autonomous() and OperatorControl() methods are called at the appropriate time
 * as the match proceeds. In the current implementation, the Autonomous code will run to
 * completion before the OperatorControl code could start. In the future the Autonomous code
 * might be spawned as a task, then killed at the end of the Autonomous period.
 */
class RobotBase
{
	friend class RobotDeleter;
public:
	static RobotBase &getInstance();
	static void setInstance(RobotBase* robot);

	bool IsEnabled();
	bool IsDisabled();
	bool IsAutonomous();
	bool IsOperatorControl();
	bool IsTest();
	bool IsNewDataAvailable();
	static void startRobotTask(FUNCPTR factory);
	static void robotTask(FUNCPTR factory, Task *task);
	virtual void StartCompetition() = 0;

protected:
	virtual ~RobotBase();
	RobotBase();

	Task *m_task;
	DriverStation *m_ds;

private:
	static RobotBase *m_instance;

	DISALLOW_COPY_AND_ASSIGN(RobotBase);
};
