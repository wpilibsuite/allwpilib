/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "DriverStation.h"
#include "AnalogInput.h"
#include "HAL/cpp/Synchronized.hpp"
#include "Timer.h"
#include "NetworkCommunication/FRCComm.h"
#include "NetworkCommunication/UsageReporting.h"
#include "MotorSafetyHelper.h"
#include "Utility.h"
#include "WPIErrors.h"
#include <string.h>
#include "Log.hpp"

// set the logging level
TLogLevel dsLogLevel = logDEBUG;
const double JOYSTICK_UNPLUGGED_MESSAGE_INTERVAL = 1.0;

#define DS_LOG(level) \
	if (level > dsLogLevel) ; \
	else Log().Get(level)

const uint32_t DriverStation::kJoystickPorts;
DriverStation* DriverStation::m_instance = NULL;

/**
 * DriverStation constructor.
 *
 * This is only called once the first time GetInstance() is called
 */
DriverStation::DriverStation()
	: m_task ("DriverStation", (FUNCPTR)DriverStation::InitTask)
	, m_newControlData(0)
	, m_packetDataAvailableMultiWait(0)
	, m_waitForDataSem(0)
	, m_userInDisabled(false)
	, m_userInAutonomous(false)
	, m_userInTeleop(false)
	, m_userInTest(false)
	, m_nextMessageTime(0)
{
	// All joysticks should default to having zero axes, povs and buttons, so
	// uninitialized memory doesn't get sent to speed controllers.
	for(unsigned int i = 0; i < kJoystickPorts; i++) {
		m_joystickAxes[i].count = 0;
		m_joystickPOVs[i].count = 0;
		m_joystickButtons[i].count = 0;
	}
	// Create a new semaphore
	m_packetDataAvailableMultiWait = initializeMultiWait();
	m_newControlData = initializeSemaphore(SEMAPHORE_EMPTY);

	m_waitForDataSem = initializeMultiWait();
	m_waitForDataMutex = initializeMutexNormal();

	m_packetDataAvailableMultiWait = initializeMultiWait();
	m_packetDataAvailableMutex = initializeMutexNormal();

	// Register that semaphore with the network communications task.
	// It will signal when new packet data is available.
	HALSetNewDataSem(m_packetDataAvailableMultiWait);

	AddToSingletonList();

	if (!m_task.Start((int32_t)this))
	{
		wpi_setWPIError(DriverStationTaskError);
	}
}

DriverStation::~DriverStation()
{
	m_task.Stop();
	m_instance = NULL;
	deleteMultiWait(m_waitForDataSem);
	// Unregister our semaphore.
	HALSetNewDataSem(0);
	deleteMultiWait(m_packetDataAvailableMultiWait);
	deleteMutex(m_packetDataAvailableMutex);
	deleteMutex(m_waitForDataMutex);
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
		takeMultiWait(m_packetDataAvailableMultiWait, m_packetDataAvailableMutex, 0);
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

	// Get the status of all of the joysticks
	for(uint8_t stick = 0; stick < kJoystickPorts; stick++) {
		HALGetJoystickAxes(stick, &m_joystickAxes[stick]);
		HALGetJoystickPOVs(stick, &m_joystickPOVs[stick]);
		HALGetJoystickButtons(stick, &m_joystickButtons[stick]);
	}
	giveSemaphore(m_newControlData);
}

/**
 * Read the battery voltage.
 *
 * @return The battery voltage.
 */
float DriverStation::GetBatteryVoltage()
{
	int32_t status = 0;
	float voltage = getVinVoltage(&status);
	wpi_setErrorWithContext(status, "getVinVoltage");

	return voltage;
}

void DriverStation::ReportJoystickUnpluggedError(std::string message) {
	double currentTime = Timer::GetFPGATimestamp();
	if (currentTime > m_nextMessageTime) {
		ReportError(message);
		m_nextMessageTime = currentTime + JOYSTICK_UNPLUGGED_MESSAGE_INTERVAL;
	}
}

/**	Returns the number of axis on a given joystick port
 *
 * @param stick The joystick port number
 * @return the number of axis on the indicated joystick
 */
int DriverStation::GetStickAxisCount(uint32_t stick)
{
    if (stick >= kJoystickPorts)
    {
        wpi_setWPIError(BadJoystickIndex);
        return 0;
    }
   	HALJoystickAxes joystickAxes;
   	HALGetJoystickAxes(stick, &joystickAxes);
   	return joystickAxes.count;
}

/**	Returns the number of POVs on a given joystick port
 *
 * @param stick The joystick port number
 * @return thenumber of POVs on the indicated joystick
 */
int DriverStation::GetStickPOVCount(uint32_t stick)
{
    if (stick >= kJoystickPorts)
    {
        wpi_setWPIError(BadJoystickIndex);
        return 0;
    }
	HALJoystickPOVs joystickPOVs;
	HALGetJoystickPOVs(stick, &joystickPOVs);
   	return joystickPOVs.count;
}

/**	Returns the number of buttons on a given joystick port
 *
 * @param stick The joystick port number
 * @return The number of buttons on the indicated joystick
 */
int DriverStation::GetStickButtonCount(uint32_t stick)
{
    if (stick >= kJoystickPorts)
    {
        wpi_setWPIError(BadJoystickIndex);
        return 0;
    }
	HALJoystickButtons joystickButtons;
	HALGetJoystickButtons(stick, &joystickButtons);
   	return joystickButtons.count;
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
	if (stick >= kJoystickPorts)
	{
		wpi_setWPIError(BadJoystickIndex);
		return 0;
	}

	if (axis >= m_joystickAxes[stick].count)
	{
		if (axis >= kMaxJoystickAxes)
			wpi_setWPIError(BadJoystickAxis);
		else
			ReportJoystickUnpluggedError("WARNING: Joystick Axis missing, check if all controllers are plugged in\n");
		return 0.0f;
	}

	int8_t value = m_joystickAxes[stick].axes[axis];

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
 * Get the state of a POV on the joystick.
 *
 * @return the angle of the POV in degrees, or -1 if the POV is not pressed.
 */
int DriverStation::GetStickPOV(uint32_t stick, uint32_t pov) {
	if (stick >= kJoystickPorts)
	{
		wpi_setWPIError(BadJoystickIndex);
		return 0;
	}

	if (pov >= m_joystickPOVs[stick].count)
	{
		if (pov >= kMaxJoystickPOVs)
			wpi_setWPIError(BadJoystickAxis);
		else
			ReportJoystickUnpluggedError("WARNING: Joystick POV missing, check if all controllers are plugged in\n");
		return 0;
	}

	return m_joystickPOVs[stick].povs[pov];
}

/**
 * The state of the buttons on the joystick.
 *
 * @param stick The joystick to read.
 * @return The state of the buttons on the joystick.
 */
uint32_t DriverStation::GetStickButtons(uint32_t stick)
{
	if (stick >= kJoystickPorts)
	{
		wpi_setWPIError(BadJoystickIndex);
		return 0;
	}

	return m_joystickButtons[stick].buttons;
}

/**
 * The state of one joystick button. Button indexes begin at 1.
 *
 * @param stick The joystick to read.
 * @param button The button index, beginning at 1.
 * @return The state of the joystick button.
 */
bool DriverStation::GetStickButton(uint32_t stick, uint8_t button)
{
	if (stick >= kJoystickPorts)
	{
		wpi_setWPIError(BadJoystickIndex);
		return false;
	}
	
	if(button > m_joystickButtons[stick].count)
	{
		ReportJoystickUnpluggedError("WARNING: Joystick Button missing, check if all controllers are plugged in\n");
		return false;
	}
	if(button == 0)
	{
		ReportJoystickUnpluggedError("ERROR: Button indexes begin at 1 in WPILib for C++ and Java");
		return false;
	}
	return ((0x1 << (button-1)) & m_joystickButtons[stick].buttons) !=0;
}

bool DriverStation::IsEnabled()
{
	HALControlWord controlWord;
    memset(&controlWord, 0, sizeof(controlWord));
	HALGetControlWord(&controlWord);
	return controlWord.enabled && controlWord.dsAttached;
}

bool DriverStation::IsDisabled()
{
	HALControlWord controlWord;
    memset(&controlWord, 0, sizeof(controlWord));
	HALGetControlWord(&controlWord);
	return !(controlWord.enabled && controlWord.dsAttached);
}

bool DriverStation::IsAutonomous()
{
	HALControlWord controlWord;
    memset(&controlWord, 0, sizeof(controlWord));
	HALGetControlWord(&controlWord);
	return controlWord.autonomous;
}

bool DriverStation::IsOperatorControl()
{
	HALControlWord controlWord;
    memset(&controlWord, 0, sizeof(controlWord));
	HALGetControlWord(&controlWord);
	return !(controlWord.autonomous || controlWord.test);
}

bool DriverStation::IsTest()
{
	HALControlWord controlWord;
	HALGetControlWord(&controlWord);
	return controlWord.test;
}

bool DriverStation::IsDSAttached()
{
	HALControlWord controlWord;
    memset(&controlWord, 0, sizeof(controlWord));
	HALGetControlWord(&controlWord);
	return controlWord.dsAttached;
}

bool DriverStation::IsSysActive()
{
	int32_t status = 0;
	bool retVal = HALGetSystemActive(&status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return retVal;
}

bool DriverStation::IsSysBrownedOut()
{
	int32_t status = 0;
	bool retVal = HALGetBrownedOut(&status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));
	return retVal;
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
	HALControlWord controlWord;
	HALGetControlWord(&controlWord);
	return controlWord.fmsAttached;
}

/**
 * Return the alliance that the driver station says it is on.
 * This could return kRed or kBlue
 * @return The Alliance enum
 */
DriverStation::Alliance DriverStation::GetAlliance()
{
	HALAllianceStationID allianceStationID;
	HALGetAllianceStation(&allianceStationID);
	switch(allianceStationID)
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
	HALAllianceStationID allianceStationID;
	HALGetAllianceStation(&allianceStationID);
	switch(allianceStationID)
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
	takeMultiWait(m_waitForDataSem, m_waitForDataMutex, SEMAPHORE_WAIT_FOREVER);
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
	float matchTime;
	HALGetMatchTime(&matchTime);
	return (double)matchTime;
}

/**
 * Report an error to the DriverStation messages window.
 * The error is also printed to the program console.
 */
void DriverStation::ReportError(std::string error)
{
	std::cout << error << std::endl;
    HALSetErrorData(error.c_str(), error.size(), 0);
}
