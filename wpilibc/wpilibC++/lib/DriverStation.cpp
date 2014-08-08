/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "DriverStation.h"
#include "AnalogInput.h"
#include "HAL/cpp/Synchronized.hpp"
#include "Timer.h"
#include "MotorSafetyHelper.h"
#include "Utility.h"
#include "WPIErrors.h"
#include <string.h>
#include "Log.hpp"

// set the logging level
TLogLevel dsLogLevel = logDEBUG;

#define DS_LOG(level) \
	if (level > dsLogLevel) ; \
	else Log().Get(level)

const uint32_t DriverStation::kJoystickPorts;
const uint32_t DriverStation::kJoystickAxes;
DriverStation* DriverStation::m_instance = NULL;

/**
 * DriverStation contructor.
 *
 * This is only called once the first time GetInstance() is called
 */
DriverStation::DriverStation()
	: m_digitalOut (0)
	, m_statusDataSemaphore (initializeMutexNormal())
	, m_task ("DriverStation", (FUNCPTR)DriverStation::InitTask)
	, m_newControlData(0)
	, m_packetDataAvailableSem (0)
	, m_waitForDataSem(0)
	, m_approxMatchTimeOffset(-1.0)
	, m_userInDisabled(false)
	, m_userInAutonomous(false)
	, m_userInTeleop(false)
	, m_userInTest(false)
{
	// Create a new semaphore
	m_packetDataAvailableSem = initializeMutexNormal();
	m_newControlData = initializeSemaphore(SEMAPHORE_EMPTY);

	// Register that semaphore with the network communications task.
	// It will signal when new packet data is available.
	HALSetNewDataSem(m_packetDataAvailableSem);

	m_waitForDataSem = initializeMultiWait();

	AddToSingletonList();

	if (!m_task.Start((int32_t)this))
	{
		wpi_setWPIError(DriverStationTaskError);
	}
}

DriverStation::~DriverStation()
{
	m_task.Stop();
	deleteMutex(m_statusDataSemaphore);
	m_instance = NULL;
	deleteMultiWait(m_waitForDataSem);
	// Unregister our semaphore.
	HALSetNewDataSem(0);
	deleteMutex(m_packetDataAvailableSem);
}

void DriverStation::InitTask(DriverStation *ds)
{
	ds->Run();
}

void DriverStation::Run()
{
	int period = 0;
	while (true)
	{
		takeMutex(m_packetDataAvailableSem);
		GetData();
		giveMultiWait(m_waitForDataSem);
		if (++period >= 4)
		{
			MotorSafetyHelper::CheckMotors();
			period = 0;
		}
		if (m_userInDisabled)
			HALNetworkCommunicationObserveUserProgramDisabled();
		if (m_userInAutonomous)
			HALNetworkCommunicationObserveUserProgramAutonomous();
		if (m_userInTeleop)
			HALNetworkCommunicationObserveUserProgramTeleop();
		if (m_userInTest)
			HALNetworkCommunicationObserveUserProgramTest();
	}
}

/**
 * Return a pointer to the singleton DriverStation.
 */
DriverStation* DriverStation::GetInstance()
{
	if (m_instance == NULL)
	{
		m_instance = new DriverStation();
	}
	return m_instance;
}

/**
 * Copy data from the DS task for the user.
 * If no new data exists, it will just be returned, otherwise
 * the data will be copied from the DS polling loop.
 */
void DriverStation::GetData()
{
	static bool lastEnabled = false;

	// Get the status data
	HALGetControlWord(&m_controlWord);

	// Get the location/alliance data
	HALGetAllianceStation(&m_allianceStationID);

	// Get the status of all of the joysticks
	for(uint8_t stick = 0; stick < kJoystickPorts; stick++) {
		uint8_t count;

		HALGetJoystickAxes(stick, &m_joystickAxes[stick], kJoystickAxes);
		HALGetJoystickButtons(stick, &m_joystickButtons[stick], &count);
	}

	if (!lastEnabled && IsEnabled())
	{
		// If starting teleop, assume that autonomous just took up 15 seconds
		if (IsAutonomous())
			m_approxMatchTimeOffset = Timer::GetFPGATimestamp();
		else
			m_approxMatchTimeOffset = Timer::GetFPGATimestamp() - 15.0;
	}
	else if (lastEnabled && !IsEnabled())
	{
		m_approxMatchTimeOffset = -1.0;
	}
	lastEnabled = IsEnabled();
	giveSemaphore(m_newControlData);
}

/**
 * Read the battery voltage.
 *
 * @return The battery voltage.
 */
float DriverStation::GetBatteryVoltage()
{
	return 0.0f; // TODO
}

/**
 * Get the value of the axis on a joystick.
 * This depends on the mapping of the joystick connected to the specified port.
 *
 * @param stick The joystick to read.
 * @param axis The analog axis value to read from the joystick.
 * @return The value of the axis on the joystick.
 */
float DriverStation::GetStickAxis(uint32_t stick, uint32_t axis)
{
	if (stick < 1 || stick > kJoystickPorts)
	{
		wpi_setWPIError(BadJoystickIndex);
		return 0;
	}

	if (axis < 1 || axis > kJoystickAxes)
	{
		wpi_setWPIError(BadJoystickAxis);
		return 0.0f;
	}

	int8_t value = m_joystickAxes[stick - 1].axes[axis - 1];

	if(value < 0)
	{
		return value / 128.0f;
	}
	else
	{
		return value / 127.0f;
	}
}

/**
 * The state of the buttons on the joystick.
 * 12 buttons (4 msb are unused) from the joystick.
 *
 * @param stick The joystick to read.
 * @return The state of the buttons on the joystick.
 */
short DriverStation::GetStickButtons(uint32_t stick)
{
	if (stick < 1 || stick > kJoystickPorts)
	{
		wpi_setWPIError(BadJoystickIndex);
		return 0;
	}

	return m_joystickButtons[stick - 1];
}

bool DriverStation::IsEnabled()
{
	return m_controlWord.enabled;
}

bool DriverStation::IsDisabled()
{
	return !m_controlWord.enabled;
}

bool DriverStation::IsAutonomous()
{
	return m_controlWord.autonomous;
}

bool DriverStation::IsOperatorControl()
{
	return !(m_controlWord.autonomous || m_controlWord.test);
}

bool DriverStation::IsTest()
{
	return m_controlWord.test;
}

/**
 * Has a new control packet from the driver station arrived since the last time this function was called?
 * Warning: If you call this function from more than one place at the same time,
 * you will not get the get the intended behavior
 * @return True if the control data has been updated since the last call.
 */
bool DriverStation::IsNewControlData()
{
	return tryTakeSemaphore(m_newControlData) == 0;
}

/**
 * Is the driver station attached to a Field Management System?
 * Note: This does not work with the Blue DS.
 * @return True if the robot is competing on a field being controlled by a Field Management System
 */
bool DriverStation::IsFMSAttached()
{
	return m_controlWord.fmsAttached;
}

/**
 * Return the alliance that the driver station says it is on.
 * This could return kRed or kBlue
 * @return The Alliance enum
 */
DriverStation::Alliance DriverStation::GetAlliance()
{
	switch(m_allianceStationID)
	{
	case kHALAllianceStationID_red1:
	case kHALAllianceStationID_red2:
	case kHALAllianceStationID_red3:
		return kRed;
	case kHALAllianceStationID_blue1:
	case kHALAllianceStationID_blue2:
	case kHALAllianceStationID_blue3:
		return kBlue;
	default:
		return kInvalid;
	}
}

/**
 * Return the driver station location on the field
 * This could return 1, 2, or 3
 * @return The location of the driver station
 */
uint32_t DriverStation::GetLocation()
{
	switch(m_allianceStationID)
	{
	case kHALAllianceStationID_red1:
	case kHALAllianceStationID_blue1:
		return 1;
	case kHALAllianceStationID_red2:
	case kHALAllianceStationID_blue2:
		return 2;
	case kHALAllianceStationID_red3:
	case kHALAllianceStationID_blue3:
		return 3;
	default:
		return 0;
	}
}

/**
 * Wait until a new packet comes from the driver station
 * This blocks on a semaphore, so the waiting is efficient.
 * This is a good way to delay processing until there is new driver station data to act on
 */
void DriverStation::WaitForData()
{
	takeMultiWait(m_waitForDataSem, SEMAPHORE_WAIT_FOREVER);
}

/**
 * Return the approximate match time
 * The FMS does not currently send the official match time to the robots
 * This returns the time since the enable signal sent from the Driver Station
 * At the beginning of autonomous, the time is reset to 0.0 seconds
 * At the beginning of teleop, the time is reset to +15.0 seconds
 * If the robot is disabled, this returns 0.0 seconds
 * Warning: This is not an official time (so it cannot be used to argue with referees)
 * @return Match time in seconds since the beginning of autonomous
 */
double DriverStation::GetMatchTime()
{
	if (m_approxMatchTimeOffset < 0.0)
		return 0.0;
	return Timer::GetFPGATimestamp() - m_approxMatchTimeOffset;
}
