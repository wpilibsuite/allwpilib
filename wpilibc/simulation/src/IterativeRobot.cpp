/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "IterativeRobot.h"

#include "DriverStation.h"
#include "SmartDashboard/SmartDashboard.h"
#include "LiveWindow/LiveWindow.h"
#include "networktables/NetworkTable.h"

//not sure what this is used for yet.
#ifdef _UNIX
	#include <unistd.h>
#endif

const double IterativeRobot::kDefaultPeriod = 0;

/**
 * Set the period for the periodic functions.
 *
 * @param period The period of the periodic function calls.  0.0 means sync to driver station control data.
 */
void IterativeRobot::SetPeriod(double period)
{
	if (period > 0.0)
	{
		// Not syncing with the DS, so start the timer for the main loop
		m_mainLoopTimer.Reset();
		m_mainLoopTimer.Start();
	}
	else
	{
		// Syncing with the DS, don't need the timer
		m_mainLoopTimer.Stop();
	}
	m_period = period;
}

/**
 * Get the period for the periodic functions.
 * Returns 0.0 if configured to syncronize with DS control data packets.
 * @return Period of the periodic function calls
 */
double IterativeRobot::GetPeriod()
{
	return m_period;
}

/**
 * Get the number of loops per second for the IterativeRobot
 * @return Frequency of the periodic function calls
 */
double IterativeRobot::GetLoopsPerSec()
{
	// If syncing to the driver station, we don't know the rate,
	//   so guess something close.
	if (m_period <= 0.0)
		return 50.0;
	return 1.0 / m_period;
}

/**
 * Provide an alternate "main loop" via StartCompetition().
 *
 * This specific StartCompetition() implements "main loop" behavior like that of the FRC
 * control system in 2008 and earlier, with a primary (slow) loop that is
 * called periodically, and a "fast loop" (a.k.a. "spin loop") that is
 * called as fast as possible with no delay between calls.
 */
void IterativeRobot::StartCompetition()
{
	LiveWindow *lw = LiveWindow::GetInstance();
	// first and one-time initialization
	SmartDashboard::init();
	NetworkTable::GetTable("LiveWindow")->GetSubTable("~STATUS~")->PutBoolean("LW Enabled", false);
	RobotInit();

	// loop forever, calling the appropriate mode-dependent function
	lw->SetEnabled(false);
	while (true)
	{
		// Call the appropriate function depending upon the current robot mode
		if (IsDisabled())
		{
			// call DisabledInit() if we are now just entering disabled mode from
			// either a different mode or from power-on
			if(!m_disabledInitialized)
			{
				lw->SetEnabled(false);
				DisabledInit();
				m_disabledInitialized = true;
				// reset the initialization flags for the other modes
				m_autonomousInitialized = false;
                m_teleopInitialized = false;
                m_testInitialized = false;
			}
			if (NextPeriodReady())
			{
				// TODO: HALNetworkCommunicationObserveUserProgramDisabled();
				DisabledPeriodic();
			}
		}
		else if (IsAutonomous())
		{
			// call AutonomousInit() if we are now just entering autonomous mode from
			// either a different mode or from power-on
			if(!m_autonomousInitialized)
			{
				lw->SetEnabled(false);
				AutonomousInit();
				m_autonomousInitialized = true;
				// reset the initialization flags for the other modes
				m_disabledInitialized = false;
                m_teleopInitialized = false;
                m_testInitialized = false;
			}
			if (NextPeriodReady())
			{
				// TODO: HALNetworkCommunicationObserveUserProgramAutonomous();
				AutonomousPeriodic();
			}
		}
        else if (IsTest())
        {
            // call TestInit() if we are now just entering test mode from
            // either a different mode or from power-on
            if(!m_testInitialized)
            {
            	lw->SetEnabled(true);
                TestInit();
                m_testInitialized = true;
                // reset the initialization flags for the other modes
                m_disabledInitialized = false;
                m_autonomousInitialized = false;
                m_teleopInitialized = false;
            }
            if (NextPeriodReady())
            {
                // TODO: HALNetworkCommunicationObserveUserProgramTest();
                TestPeriodic();
            }
        }
		else
		{
			// call TeleopInit() if we are now just entering teleop mode from
			// either a different mode or from power-on
			if(!m_teleopInitialized)
			{
				lw->SetEnabled(false);
				TeleopInit();
				m_teleopInitialized = true;
				// reset the initialization flags for the other modes
				m_disabledInitialized = false;
                m_autonomousInitialized = false;
                m_testInitialized = false;
                Scheduler::GetInstance()->SetEnabled(true);
			}
			if (NextPeriodReady())
			{
				// TODO: HALNetworkCommunicationObserveUserProgramTeleop();
				TeleopPeriodic();
			}
		}
		// wait for driver station data so the loop doesn't hog the CPU
		m_ds.WaitForData();
	}
}

/**
 * Determine if the periodic functions should be called.
 *
 * If m_period > 0.0, call the periodic function every m_period as compared
 * to Timer.Get().  If m_period == 0.0, call the periodic functions whenever
 * a packet is received from the Driver Station, or about every 20ms.
 *
 * @todo Decide what this should do if it slips more than one cycle.
 */

bool IterativeRobot::NextPeriodReady()
{
	if (m_period > 0.0)
	{
		return m_mainLoopTimer.HasPeriodPassed(m_period);
	}
	else
	{
		// XXX: BROKEN! return m_ds->IsNewControlData();
	}
    return true;
}

/**
 * Robot-wide initialization code should go here.
 *
 * Users should override this method for default Robot-wide initialization which will
 * be called when the robot is first powered on.  It will be called exactly 1 time.
 */
void IterativeRobot::RobotInit()
{
	printf("Default %s() method... Overload me!\n", __FUNCTION__);
}

/**
 * Initialization code for disabled mode should go here.
 *
 * Users should override this method for initialization code which will be called each time
 * the robot enters disabled mode.
 */
void IterativeRobot::DisabledInit()
{
	printf("Default %s() method... Overload me!\n", __FUNCTION__);
}

/**
 * Initialization code for autonomous mode should go here.
 *
 * Users should override this method for initialization code which will be called each time
 * the robot enters autonomous mode.
 */
void IterativeRobot::AutonomousInit()
{
	printf("Default %s() method... Overload me!\n", __FUNCTION__);
}

/**
 * Initialization code for teleop mode should go here.
 *
 * Users should override this method for initialization code which will be called each time
 * the robot enters teleop mode.
 */
void IterativeRobot::TeleopInit()
{
    printf("Default %s() method... Overload me!\n", __FUNCTION__);
}

/**
 * Initialization code for test mode should go here.
 *
 * Users should override this method for initialization code which will be called each time
 * the robot enters test mode.
 */
void IterativeRobot::TestInit()
{
    printf("Default %s() method... Overload me!\n", __FUNCTION__);
}

/**
 * Periodic code for disabled mode should go here.
 *
 * Users should override this method for code which will be called periodically at a regular
 * rate while the robot is in disabled mode.
 */
void IterativeRobot::DisabledPeriodic()
{
	static bool firstRun = true;
	if (firstRun)
	{
		printf("Default %s() method... Overload me!\n", __FUNCTION__);
		firstRun = false;
	}
}

/**
 * Periodic code for autonomous mode should go here.
 *
 * Users should override this method for code which will be called periodically at a regular
 * rate while the robot is in autonomous mode.
 */
void IterativeRobot::AutonomousPeriodic()
{
	static bool firstRun = true;
	if (firstRun)
	{
		printf("Default %s() method... Overload me!\n", __FUNCTION__);
		firstRun = false;
	}
}

/**
 * Periodic code for teleop mode should go here.
 *
 * Users should override this method for code which will be called periodically at a regular
 * rate while the robot is in teleop mode.
 */
void IterativeRobot::TeleopPeriodic()
{
	static bool firstRun = true;
	if (firstRun)
	{
		printf("Default %s() method... Overload me!\n", __FUNCTION__);
		firstRun = false;
	}
}

/**
 * Periodic code for test mode should go here.
 *
 * Users should override this method for code which will be called periodically at a regular
 * rate while the robot is in test mode.
 */
void IterativeRobot::TestPeriodic()
{
    static bool firstRun = true;
    if (firstRun)
    {
        printf("Default %s() method... Overload me!\n", __FUNCTION__);
        firstRun = false;
    }
}

