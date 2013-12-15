/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "RobotBase.h"

#include "DriverStation.h"
#include "NetworkCommunication/FRCComm.h"
#include "NetworkCommunication/symModuleLink.h"
#include "NetworkCommunication/UsageReporting.h"
#include "Utility.h"
#include <cstring>
#include "HAL/HAL.h"

#ifdef __vxworks
// VXWorks needs som special unloading code
#include <moduleLib.h>
#include <unldLib.h>
#include <taskLib.h>
#endif

RobotBase* RobotBase::m_instance = NULL;

void RobotBase::setInstance(RobotBase* robot)
{
	wpi_assert(m_instance == NULL);
	m_instance = robot;
}

RobotBase &RobotBase::getInstance()
{
	return *m_instance;
}

/**
 * Constructor for a generic robot program.
 * User code should be placed in the constuctor that runs before the Autonomous or Operator
 * Control period starts. The constructor will run to completion before Autonomous is entered.
 * 
 * This must be used to ensure that the communications code starts. In the future it would be
 * nice to put this code into it's own task that loads on boot so ensure that it runs.
 */
RobotBase::RobotBase()
	: m_task (NULL)
	, m_ds (NULL)
{
	m_ds = DriverStation::GetInstance();
}

/**
 * Free the resources for a RobotBase class.
 * This includes deleting all classes that might have been allocated as Singletons to they
 * would never be deleted except here.
 */
RobotBase::~RobotBase()
{
	SensorBase::DeleteSingletons();
	delete m_task;
	m_task = NULL;
	m_instance = NULL;
}

/**
 * Check on the overall status of the system.
 * 
 * @return Is the system active (i.e. PWM motor outputs, etc. enabled)?
 */
bool RobotBase::IsSystemActive()
{
	return m_watchdog.IsSystemActive();
}

/**
 * Return the instance of the Watchdog timer.
 * Get the watchdog timer so the user program can either disable it or feed it when
 * necessary.
 */
Watchdog &RobotBase::GetWatchdog()
{
	return m_watchdog;
}

/**
 * Determine if the Robot is currently enabled.
 * @return True if the Robot is currently enabled by the field controls.
 */
bool RobotBase::IsEnabled()
{
	return m_ds->IsEnabled();
}

/**
 * Determine if the Robot is currently disabled.
 * @return True if the Robot is currently disabled by the field controls.
 */
bool RobotBase::IsDisabled()
{
	return m_ds->IsDisabled();
}

/**
 * Determine if the robot is currently in Autnomous mode.
 * @return True if the robot is currently operating Autonomously as determined by the field controls.
 */
bool RobotBase::IsAutonomous()
{
	return m_ds->IsAutonomous();
}

/**
 * Determine if the robot is currently in Operator Control mode.
 * @return True if the robot is currently operating in Tele-Op mode as determined by the field controls.
 */
bool RobotBase::IsOperatorControl()
{
	return m_ds->IsOperatorControl();
}

/**
 * Determine if the robot is currently in Test mode.
 * @return True if the robot is currently running tests as determined by the field controls.
 */
bool RobotBase::IsTest()
{
    return m_ds->IsTest();
}

/**
 * Indicates if new data is available from the driver station.
 * @return Has new data arrived over the network since the last time this function was called?
 */
bool RobotBase::IsNewDataAvailable()
{
	return m_ds->IsNewControlData();
}

/**
 * Static interface that will start the competition in the new task.
 */
void RobotBase::robotTask(FUNCPTR factory, Task *task)
{
	RobotBase::setInstance((RobotBase*)factory());
	RobotBase::getInstance().m_task = task;
	RobotBase::getInstance().StartCompetition();
}

/**
 * 
 * Start the robot code.
 * This function starts the robot code running by spawning a task. Currently tasks seemed to be
 * started by LVRT without setting the VX_FP_TASK flag so floating point context is not saved on
 * interrupts. Therefore the program experiences hard to debug and unpredictable results. So the
 * LVRT code starts this function, and it, in turn, starts the actual user program.
 */
void RobotBase::startRobotTask(FUNCPTR factory)
{
#ifdef SVN_REV
	if (strlen(SVN_REV))
	{
		printf("WPILib was compiled from SVN revision %s\n", SVN_REV);
	}
	else
	{
		printf("WPILib was compiled from a location that is not source controlled.\n");
	}
#else
	printf("WPILib was compiled without -D'SVN_REV=nnnn'\n");
#endif

#ifdef __vxworks
	// Check for startup code already running
	int32_t oldId = taskNameToId(const_cast<char*>("FRC_RobotTask"));
	if (oldId != ERROR)
	{
		// Find the startup code module.
		char moduleName[256];
		moduleNameFindBySymbolName("FRC_UserProgram_StartupLibraryInit", moduleName);
		MODULE_ID startupModId = moduleFindByName(moduleName);
		if (startupModId != NULL)
		{
			// Remove the startup code.
			unldByModuleId(startupModId, 0);
			printf("!!!   Error: Default code was still running... It was unloaded for you... Please try again.\n");
			return;
		}
		// This case should no longer get hit.
		printf("!!!   Error: Other robot code is still running... Unload it and then try again.\n");
		return;
	}
#endif

	// Let the framework know that we are starting a new user program so the Driver Station can disable.
	FRC_NetworkCommunication_observeUserProgramStarting();

	// Let the Usage Reporting framework know that there is a C++ program running
	nUsageReporting::report(nUsageReporting::kResourceType_Language, nUsageReporting::kLanguage_CPlusPlus);

	// Start robot task
	// This is done to ensure that the C++ robot task is spawned with the floating point
	// context save parameter.
	Task *task = new Task("RobotTask", (FUNCPTR)RobotBase::robotTask, Task::kDefaultPriority, 64000);
	task->Start((int32_t)factory, (int32_t)task);
}

/**
 * This class exists for the sole purpose of getting its destructor called when the module unloads.
 * Before the module is done unloading, we need to delete the RobotBase derived singleton.  This should delete
 * the other remaining singletons that were registered.  This should also stop all tasks that are using
 * the Task class.
 */
class RobotDeleter
{
public:
	RobotDeleter() {}
	~RobotDeleter()
	{
		delete &RobotBase::getInstance();
	}
};
static RobotDeleter g_robotDeleter;
