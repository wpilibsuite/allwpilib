/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "DriverStation.h"
#include "AnalogChannel.h"
#include "HAL/cpp/Synchronized.h"
#include "Timer.h"
//#include "NetworkCommunication/FRCComm.h"
//#include "NetworkCommunication/UsageReporting.h"
#include "MotorSafetyHelper.h"
#include "Utility.h"
#include "WPIErrors.h"
#include <string.h>
#include "Log.h"

// set the logging level
TLogLevel dsLogLevel = logDEBUG;

#define DS_LOG(level) \
    if (level > dsLogLevel) ; \
    else Log().Get(level)

const uint32_t DriverStation::kBatteryModuleNumber;
const uint32_t DriverStation::kBatteryChannel;
const uint32_t DriverStation::kJoystickPorts;
const uint32_t DriverStation::kJoystickAxes;
constexpr float DriverStation::kUpdatePeriod;
DriverStation* DriverStation::m_instance = NULL;
uint8_t DriverStation::m_updateNumber = 0;

/**
 * DriverStation contructor.
 * 
 * This is only called once the first time GetInstance() is called
 */
DriverStation::DriverStation()
	: m_controlData (NULL)
	, m_digitalOut (0)
	, m_batteryChannel (NULL)
	, m_statusDataSemaphore (initializeMutexNormal())
	, m_task ("DriverStation", (FUNCPTR)DriverStation::InitTask)
	, m_dashboardHigh(m_statusDataSemaphore)
	, m_dashboardLow(m_statusDataSemaphore)
	, m_dashboardInUseHigh(&m_dashboardHigh)
	, m_dashboardInUseLow(&m_dashboardLow)
	, m_newControlData(0)
	, m_packetDataAvailableSem (0)
	, m_enhancedIO()
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

	m_controlData = new HALCommonControlData;

	// initialize packet number and control words to zero;
	m_controlData->packetIndex = 0;
	m_controlData->control = 0;

	// set all joystick axis values to neutral; buttons to OFF
	m_controlData->stick0Axis1 = m_controlData->stick0Axis2 = m_controlData->stick0Axis3 = 0;
	m_controlData->stick1Axis1 = m_controlData->stick1Axis2 = m_controlData->stick1Axis3 = 0;
	m_controlData->stick2Axis1 = m_controlData->stick2Axis2 = m_controlData->stick2Axis3 = 0;
	m_controlData->stick3Axis1 = m_controlData->stick3Axis2 = m_controlData->stick3Axis3 = 0;
	m_controlData->stick0Axis4 = m_controlData->stick0Axis5 = m_controlData->stick0Axis6 = 0;
	m_controlData->stick1Axis4 = m_controlData->stick1Axis5 = m_controlData->stick1Axis6 = 0;
	m_controlData->stick2Axis4 = m_controlData->stick2Axis5 = m_controlData->stick2Axis6 = 0;
	m_controlData->stick3Axis4 = m_controlData->stick3Axis5 = m_controlData->stick3Axis6 = 0;
	m_controlData->stick0Buttons = 0;
	m_controlData->stick1Buttons = 0;
	m_controlData->stick2Buttons = 0;
	m_controlData->stick3Buttons = 0;

	// initialize the analog and digital data.
	m_controlData->analog1 = 0;
	m_controlData->analog2 = 0;
	m_controlData->analog3 = 0;
	m_controlData->analog4 = 0;
	m_controlData->dsDigitalIn = 0;

	m_batteryChannel = new AnalogChannel(kBatteryModuleNumber, kBatteryChannel);

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
	delete m_batteryChannel;
	delete m_controlData;
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
		SetData();
		m_enhancedIO.UpdateData();
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

	HALGetCommonControlData(m_controlData, HAL_WAIT_FOREVER);

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
 * Copy status data from the DS task for the user.
 */
void DriverStation::SetData()
{
	char *userStatusDataHigh;
	int32_t userStatusDataHighSize;
	char *userStatusDataLow;
	int32_t userStatusDataLowSize;

	Synchronized sync(m_statusDataSemaphore);

	m_dashboardInUseHigh->GetStatusBuffer(&userStatusDataHigh, &userStatusDataHighSize);
	m_dashboardInUseLow->GetStatusBuffer(&userStatusDataLow, &userStatusDataLowSize);
	HALSetStatusData(GetBatteryVoltage(), m_digitalOut, m_updateNumber,
		userStatusDataHigh, userStatusDataHighSize, userStatusDataLow, userStatusDataLowSize, HAL_WAIT_FOREVER);
	
	m_dashboardInUseHigh->Flush();
	m_dashboardInUseLow->Flush();
}

/**
 * Read the battery voltage from the specified AnalogChannel.
 * 
 * This accessor assumes that the battery voltage is being measured
 * through the voltage divider on an analog breakout.
 * 
 * @return The battery voltage.
 */
float DriverStation::GetBatteryVoltage()
{
	if (m_batteryChannel == NULL)
		wpi_setWPIError(NullParameter);

	// The Analog bumper has a voltage divider on the battery source.
	// Vbatt *--/\/\/\--* Vsample *--/\/\/\--* Gnd
	//         680 Ohms            1000 Ohms
	return m_batteryChannel->GetAverageVoltage() * (1680.0 / 1000.0);
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
	if (axis < 1 || axis > kJoystickAxes)
	{
		wpi_setWPIError(BadJoystickAxis);
		return 0.0;
	}

	int8_t value;
	switch (stick)
	{
		case 1:
			value = m_controlData->stick0Axes[axis-1];
			break;
		case 2:
			value = m_controlData->stick1Axes[axis-1];
			break;
		case 3:
			value = m_controlData->stick2Axes[axis-1];
			break;
		case 4:
			value = m_controlData->stick3Axes[axis-1];
			break;
		default:
			wpi_setWPIError(BadJoystickIndex);
			return 0.0;
	}
	
	float result;
	if (value < 0)
		result = ((float) value) / 128.0;
	else
		result = ((float) value) / 127.0;
	wpi_assert(result <= 1.0 && result >= -1.0);
	if (result > 1.0)
		result = 1.0;
	else if (result < -1.0)
		result = -1.0;
	return result;
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
	if (stick < 1 || stick > 4)
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "stick must be between 1 and 4");

	switch (stick)
	{
	case 1:
		return m_controlData->stick0Buttons;
	case 2:
		return m_controlData->stick1Buttons;
	case 3:
		return m_controlData->stick2Buttons;
	case 4:
		return m_controlData->stick3Buttons;
	}
	return 0;
}

// 5V divided by 10 bits
#define kDSAnalogInScaling ((float)(5.0 / 1023.0))

/**
 * Get an analog voltage from the Driver Station.
 * The analog values are returned as voltage values for the Driver Station analog inputs.
 * These inputs are typically used for advanced operator interfaces consisting of potentiometers
 * or resistor networks representing values on a rotary switch.
 * 
 * @param channel The analog input channel on the driver station to read from. Valid range is 1 - 4.
 * @return The analog voltage on the input.
 */
float DriverStation::GetAnalogIn(uint32_t channel)
{
	if (channel < 1 || channel > 4)
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "channel must be between 1 and 4");

	static uint8_t reported_mask = 0;
	if (!(reported_mask & (1 >> channel)))
	{
		HALReport(HALUsageReporting::kResourceType_DriverStationCIO, channel, HALUsageReporting::kDriverStationCIO_Analog);
		reported_mask |= (1 >> channel);
	}

	switch (channel)
	{
	case 1:
		return kDSAnalogInScaling * m_controlData->analog1;
	case 2:
		return kDSAnalogInScaling * m_controlData->analog2;
	case 3:
		return kDSAnalogInScaling * m_controlData->analog3;
	case 4:
		return kDSAnalogInScaling * m_controlData->analog4;
	}
	return 0.0;
}

/**
 * Get values from the digital inputs on the Driver Station.
 * Return digital values from the Drivers Station. These values are typically used for buttons
 * and switches on advanced operator interfaces.
 * @param channel The digital input to get. Valid range is 1 - 8.
 */
bool DriverStation::GetDigitalIn(uint32_t channel)
{
	if (channel < 1 || channel > 8)
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "channel must be between 1 and 8");

	static uint8_t reported_mask = 0;
	if (!(reported_mask & (1 >> channel)))
	{
		HALReport(HALUsageReporting::kResourceType_DriverStationCIO, channel, HALUsageReporting::kDriverStationCIO_DigitalIn);
		reported_mask |= (1 >> channel);
	}

	return ((m_controlData->dsDigitalIn >> (channel-1)) & 0x1) ? true : false;
}

/**
 * Set a value for the digital outputs on the Driver Station.
 * 
 * Control digital outputs on the Drivers Station. These values are typically used for
 * giving feedback on a custom operator station such as LEDs.
 * 
 * @param channel The digital output to set. Valid range is 1 - 8.
 * @param value The state to set the digital output.
 */
void DriverStation::SetDigitalOut(uint32_t channel, bool value) 
{
	if (channel < 1 || channel > 8)
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "channel must be between 1 and 8");

	static uint8_t reported_mask = 0;
	if (!(reported_mask & (1 >> channel)))
	{
		HALReport(HALUsageReporting::kResourceType_DriverStationCIO, channel,HALUsageReporting::kDriverStationCIO_DigitalOut);
		reported_mask |= (1 >> channel);
	}

	m_digitalOut &= ~(0x1 << (channel-1));
	m_digitalOut |= ((uint8_t)value << (channel-1));
}

/**
 * Get a value that was set for the digital outputs on the Driver Station.
 * @param channel The digital ouput to monitor. Valid range is 1 through 8.
 * @return A digital value being output on the Drivers Station.
 */
bool DriverStation::GetDigitalOut(uint32_t channel) 
{
	if (channel < 1 || channel > 8)
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "channel must be between 1 and 8");

	return ((m_digitalOut >> (channel-1)) & 0x1) ? true : false;
}

bool DriverStation::IsEnabled()
{
	return m_controlData->enabled;
}

bool DriverStation::IsDisabled()
{
	return !m_controlData->enabled;
}

bool DriverStation::IsAutonomous()
{
	return m_controlData->autonomous;
}

bool DriverStation::IsOperatorControl()
{
	return !(m_controlData->autonomous || m_controlData->test);
}

bool DriverStation::IsTest()
{
	return m_controlData->test;
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
	return m_controlData->fmsAttached;
}

/**
 * Return the DS packet number.
 * The packet number is the index of this set of data returned by the driver station.
 * Each time new data is received, the packet number (included with the sent data) is returned.
 * @return The driver station packet number
 */
uint32_t DriverStation::GetPacketNumber()
{
	return m_controlData->packetIndex;
}

/**
 * Return the alliance that the driver station says it is on.
 * This could return kRed or kBlue
 * @return The Alliance enum
 */
DriverStation::Alliance DriverStation::GetAlliance()
{
	if (m_controlData->dsID_Alliance == 'R') return kRed;
	if (m_controlData->dsID_Alliance == 'B') return kBlue;
	wpi_assert(false);
	return kInvalid;
}

/**
 * Return the driver station location on the field
 * This could return 1, 2, or 3
 * @return The location of the driver station
 */
uint32_t DriverStation::GetLocation()
{
	wpi_assert ((m_controlData->dsID_Position >= '1') && (m_controlData->dsID_Position <= '3'));
	return m_controlData->dsID_Position - '0';
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

/**
 * Return the team number that the Driver Station is configured for
 * @return The team number
 */
uint16_t DriverStation::GetTeamNumber()
{
	return m_controlData->teamID;
}
