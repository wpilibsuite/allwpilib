/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __DRIVER_STATION_H__
#define __DRIVER_STATION_H__

#include "Dashboard.h"
#include "DriverStationEnhancedIO.h"
#include "SensorBase.h"
#include "Task.h"

struct FRCCommonControlData;
class AnalogChannel;

/**
 * Provide access to the network communication data to / from the Driver Station.
 */
class DriverStation : public SensorBase
{
public:
	enum Alliance {kRed, kBlue, kInvalid};

	virtual ~DriverStation();
	static DriverStation *GetInstance();

	static const uint32_t kBatteryModuleNumber = 1;
	static const uint32_t kBatteryChannel = 8;
	static const uint32_t kJoystickPorts = 4;
	static const uint32_t kJoystickAxes = 6;

	float GetStickAxis(uint32_t stick, uint32_t axis);
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
	bool IsNewControlData();
	bool IsFMSAttached();

	uint32_t GetPacketNumber();
	Alliance GetAlliance();
	uint32_t GetLocation();
	void WaitForData();
	double GetMatchTime();
	float GetBatteryVoltage();
	uint16_t GetTeamNumber();

	// Get the default dashboard packers. These instances stay around even after
	// a call to SetHigh|LowPriorityDashboardPackerToUse() changes which packer
	// is in use. You can restore the default high priority packer by calling
	// SetHighPriorityDashboardPackerToUse(&GetHighPriorityDashboardPacker()).
	Dashboard& GetHighPriorityDashboardPacker() { return m_dashboardHigh; }
	Dashboard& GetLowPriorityDashboardPacker() { return m_dashboardLow; }

	// Get/set the dashboard packers to use. This can sideline or restore the
	// default packers. Initializing SmartDashboard changes the high priority
	// packer in use so beware that the default packer will then be idle. These
	// methods support any kind of DashboardBase, e.g. a Dashboard or a
	// SmartDashboard.
	DashboardBase* GetHighPriorityDashboardPackerInUse() { return m_dashboardInUseHigh; }
	DashboardBase* GetLowPriorityDashboardPackerInUse() { return m_dashboardInUseLow; }
	void SetHighPriorityDashboardPackerToUse(DashboardBase* db) { m_dashboardInUseHigh = db; }
	void SetLowPriorityDashboardPackerToUse(DashboardBase* db) { m_dashboardInUseLow = db; }

	DriverStationEnhancedIO& GetEnhancedIO() { return m_enhancedIO; }

	void IncrementUpdateNumber() { m_updateNumber++; }
	MUTEX_ID GetUserStatusDataSem() { return m_statusDataSemaphore; }

	/** Only to be used to tell the Driver Station what code you claim to be executing
	 *   for diagnostic purposes only
	 * @param entering If true, starting disabled code; if false, leaving disabled code */
	void InDisabled(bool entering) {m_userInDisabled=entering;}
	/** Only to be used to tell the Driver Station what code you claim to be executing
	 *   for diagnostic purposes only
	 * @param entering If true, starting autonomous code; if false, leaving autonomous code */
	void InAutonomous(bool entering) {m_userInAutonomous=entering;}
    /** Only to be used to tell the Driver Station what code you claim to be executing
     *   for diagnostic purposes only
     * @param entering If true, starting teleop code; if false, leaving teleop code */
    void InOperatorControl(bool entering) {m_userInTeleop=entering;}
    /** Only to be used to tell the Driver Station what code you claim to be executing
     *   for diagnostic purposes only
     * @param entering If true, starting test code; if false, leaving test code */
    void InTest(bool entering) {m_userInTest=entering;}

protected:
	DriverStation();

	void GetData();
	void SetData();

private:
	static void InitTask(DriverStation *ds);
	static DriverStation *m_instance;
	static uint8_t m_updateNumber;
	///< TODO: Get rid of this and use the semaphore signaling
	static constexpr float kUpdatePeriod = 0.02;

	void Run();

	struct FRCCommonControlData *m_controlData;
	uint8_t m_digitalOut;
	AnalogChannel *m_batteryChannel;
	MUTEX_ID m_statusDataSemaphore;
	Task m_task;
	Dashboard m_dashboardHigh;  // the default dashboard packers
	Dashboard m_dashboardLow;
	DashboardBase* m_dashboardInUseHigh;  // the current dashboard packers in use
	DashboardBase* m_dashboardInUseLow;
	SEMAPHORE_ID m_newControlData;
	MUTEX_ID m_packetDataAvailableSem;
	DriverStationEnhancedIO m_enhancedIO;
	MULTIWAIT_ID m_waitForDataSem;
	double m_approxMatchTimeOffset;
	bool m_userInDisabled;
	bool m_userInAutonomous;
    bool m_userInTeleop;
    bool m_userInTest;
};

#endif

