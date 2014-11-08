/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "simulation/msgs/msgs.h"
#include <gazebo/transport/transport.hh>
#include "SensorBase.h"
#include "RobotState.h"
#include "Task.h"

struct HALCommonControlData;
class AnalogInput;

using namespace gazebo;

/**
 * Provide access to the network communication data to / from the Driver Station.
 */
class DriverStation : public SensorBase, public RobotStateInterface
{
public:
	enum Alliance
	{
		kRed,
		kBlue,
		kInvalid
	};

	virtual ~DriverStation();
	static DriverStation *GetInstance();
	static void ReportError(std::string error);

	static const uint32_t kBatteryChannel = 7;
	static const uint32_t kJoystickPorts = 4;
	static const uint32_t kJoystickAxes = 6;

	float GetStickAxis(uint32_t stick, uint32_t axis);
	bool GetStickButton(uint32_t stick, uint32_t button);
	short GetStickButtons(uint32_t stick);

	float GetAnalogIn(uint32_t channel);
	bool GetDigitalIn(uint32_t channel);
	void SetDigitalOut(uint32_t channel, bool value);
	bool GetDigitalOut(uint32_t channel);

	bool IsEnabled();
	bool IsDisabled();
	bool IsAutonomous();
	bool IsOperatorControl();
	bool IsTest();
	bool IsFMSAttached();

	uint32_t GetPacketNumber();
	Alliance GetAlliance();
	uint32_t GetLocation();
	void WaitForData();
	double GetMatchTime();
	float GetBatteryVoltage();
	uint16_t GetTeamNumber();
	


	void IncrementUpdateNumber()
	{
		m_updateNumber++;
	}

	/** Only to be used to tell the Driver Station what code you claim to be executing
	 *   for diagnostic purposes only
	 * @param entering If true, starting disabled code; if false, leaving disabled code */
	void InDisabled(bool entering)
	{
		m_userInDisabled = entering;
	}
	/** Only to be used to tell the Driver Station what code you claim to be executing
	 *   for diagnostic purposes only
	 * @param entering If true, starting autonomous code; if false, leaving autonomous code */
	void InAutonomous(bool entering)
	{
		m_userInAutonomous = entering;
	}
	/** Only to be used to tell the Driver Station what code you claim to be executing
	*   for diagnostic purposes only
	* @param entering If true, starting teleop code; if false, leaving teleop code */
	void InOperatorControl(bool entering)
	{
		m_userInTeleop = entering;
	}
	/** Only to be used to tell the Driver Station what code you claim to be executing
	*   for diagnostic purposes only
	* @param entering If true, starting test code; if false, leaving test code */
	void InTest(bool entering)
	{
		m_userInTest = entering;
	}

protected:
	DriverStation();

private:
	static void InitTask(DriverStation *ds);
	static DriverStation *m_instance;
	static uint8_t m_updateNumber;
	///< TODO: Get rid of this and use the semaphore signaling
	static constexpr float kUpdatePeriod = 0.02;

    void stateCallback(const msgs::ConstDriverStationPtr &msg);
    void joystickCallback(const msgs::ConstJoystickPtr &msg, int i);
    void joystickCallback0(const msgs::ConstJoystickPtr &msg);
    void joystickCallback1(const msgs::ConstJoystickPtr &msg);
    void joystickCallback2(const msgs::ConstJoystickPtr &msg);
    void joystickCallback3(const msgs::ConstJoystickPtr &msg);
    void joystickCallback4(const msgs::ConstJoystickPtr &msg);
    void joystickCallback5(const msgs::ConstJoystickPtr &msg);

	uint8_t m_digitalOut;
	MULTIWAIT_ID m_waitForDataSem;
	MUTEX_ID m_waitForDataMutex;
	MUTEX_ID m_stateSemaphore;
	MUTEX_ID m_joystickSemaphore;
	double m_approxMatchTimeOffset;
	bool m_userInDisabled;
	bool m_userInAutonomous;
	bool m_userInTeleop;
	bool m_userInTest;

    transport::SubscriberPtr stateSub;
    transport::SubscriberPtr joysticksSub[6];
    msgs::DriverStationPtr state;
    msgs::JoystickPtr joysticks[6];
};
