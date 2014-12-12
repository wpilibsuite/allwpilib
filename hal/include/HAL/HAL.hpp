/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2013. All Rights Reserved.  			      */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#ifdef __vxworks
#include <vxWorks.h>
#else
#include <stdint.h>
#endif
#include <cmath>

#include "Accelerometer.hpp"
#include "Analog.hpp"
#include "Compressor.hpp"
#include "Digital.hpp"
#include "Solenoid.hpp"
#include "Notifier.hpp"
#include "Interrupts.hpp"
#include "Errors.hpp"
#include "PDP.hpp"
#include "Power.hpp"
#include "SerialPort.hpp"

#include "Utilities.hpp"
#include "Semaphore.hpp"
#include "Task.hpp"

#define HAL_IO_CONFIG_DATA_SIZE 32
#define HAL_SYS_STATUS_DATA_SIZE 44
#define HAL_USER_STATUS_DATA_SIZE (984 - HAL_IO_CONFIG_DATA_SIZE - HAL_SYS_STATUS_DATA_SIZE)

#define HALFRC_NetworkCommunication_DynamicType_DSEnhancedIO_Input 17
#define HALFRC_NetworkCommunication_DynamicType_DSEnhancedIO_Output 18
#define HALFRC_NetworkCommunication_DynamicType_Kinect_Header 19
#define HALFRC_NetworkCommunication_DynamicType_Kinect_Extra1 20
#define HALFRC_NetworkCommunication_DynamicType_Kinect_Vertices1 21
#define HALFRC_NetworkCommunication_DynamicType_Kinect_Extra2 22
#define HALFRC_NetworkCommunication_DynamicType_Kinect_Vertices2 23
#define HALFRC_NetworkCommunication_DynamicType_Kinect_Joystick 24
#define HALFRC_NetworkCommunication_DynamicType_Kinect_Custom 25

namespace HALUsageReporting
{
	enum tResourceType
	{
		kResourceType_Controller,
		kResourceType_Module,
		kResourceType_Language,
		kResourceType_CANPlugin,
		kResourceType_Accelerometer,
		kResourceType_ADXL345,
		kResourceType_AnalogChannel,
		kResourceType_AnalogTrigger,
		kResourceType_AnalogTriggerOutput,
		kResourceType_CANJaguar,
		kResourceType_Compressor,
		kResourceType_Counter,
		kResourceType_Dashboard,
		kResourceType_DigitalInput,
		kResourceType_DigitalOutput,
		kResourceType_DriverStationCIO,
		kResourceType_DriverStationEIO,
		kResourceType_DriverStationLCD,
		kResourceType_Encoder,
		kResourceType_GearTooth,
		kResourceType_Gyro,
		kResourceType_I2C,
		kResourceType_Framework,
		kResourceType_Jaguar,
		kResourceType_Joystick,
		kResourceType_Kinect,
		kResourceType_KinectStick,
		kResourceType_PIDController,
		kResourceType_Preferences,
		kResourceType_PWM,
		kResourceType_Relay,
		kResourceType_RobotDrive,
		kResourceType_SerialPort,
		kResourceType_Servo,
		kResourceType_Solenoid,
		kResourceType_SPI,
		kResourceType_Task,
		kResourceType_Ultrasonic,
		kResourceType_Victor,
		kResourceType_Button,
		kResourceType_Command,
		kResourceType_AxisCamera,
		kResourceType_PCVideoServer,
		kResourceType_SmartDashboard,
		kResourceType_Talon,
		kResourceType_HiTechnicColorSensor,
		kResourceType_HiTechnicAccel,
		kResourceType_HiTechnicCompass,
		kResourceType_SRF08,
	};

	enum tInstances
	{
		kLanguage_LabVIEW = 1,
		kLanguage_CPlusPlus = 2,
		kLanguage_Java = 3,
		kLanguage_Python = 4,

		kCANPlugin_BlackJagBridge = 1,
		kCANPlugin_2CAN = 2,

		kFramework_Iterative = 1,
		kFramework_Simple = 2,

		kRobotDrive_ArcadeStandard = 1,
		kRobotDrive_ArcadeButtonSpin = 2,
		kRobotDrive_ArcadeRatioCurve = 3,
		kRobotDrive_Tank = 4,
		kRobotDrive_MecanumPolar = 5,
		kRobotDrive_MecanumCartesian = 6,

		kDriverStationCIO_Analog = 1,
		kDriverStationCIO_DigitalIn = 2,
		kDriverStationCIO_DigitalOut = 3,

		kDriverStationEIO_Acceleration = 1,
		kDriverStationEIO_AnalogIn = 2,
		kDriverStationEIO_AnalogOut = 3,
		kDriverStationEIO_Button = 4,
		kDriverStationEIO_LED = 5,
		kDriverStationEIO_DigitalIn = 6,
		kDriverStationEIO_DigitalOut = 7,
		kDriverStationEIO_FixedDigitalOut = 8,
		kDriverStationEIO_PWM = 9,
		kDriverStationEIO_Encoder = 10,
		kDriverStationEIO_TouchSlider = 11,

		kADXL345_SPI = 1,
		kADXL345_I2C = 2,

		kCommand_Scheduler = 1,

		kSmartDashboard_Instance = 1,
	};
}

struct HALControlWord {
	uint32_t enabled : 1;
	uint32_t autonomous : 1;
	uint32_t test :1;
	uint32_t eStop : 1;
	uint32_t fmsAttached:1;
	uint32_t dsAttached:1;
	uint32_t control_reserved : 26;
};

enum HALAllianceStationID {
	kHALAllianceStationID_red1,
	kHALAllianceStationID_red2,
	kHALAllianceStationID_red3,
	kHALAllianceStationID_blue1,
	kHALAllianceStationID_blue2,
	kHALAllianceStationID_blue3,
};

/* The maximum number of axes that will be stored in a single HALJoystickAxes
	struct. This is used for allocating buffers, not bounds checking, since
	there are usually less axes in practice. */
static constexpr size_t kMaxJoystickAxes = 12;
static constexpr size_t kMaxJoystickPOVs = 12;

struct HALJoystickAxes {
	uint16_t count;
	int16_t axes[kMaxJoystickAxes];
};

struct HALJoystickPOVs {
	uint16_t count;
	int16_t povs[kMaxJoystickPOVs];
};

struct HALJoystickButtons {
	uint32_t buttons;
	uint8_t count;
};

struct HALJoystickDescriptor {
	uint8_t isXbox;
	uint8_t type;
	char name[256];
	uint8_t axisCount;
	uint8_t axisTypes;
	uint8_t buttonCount;
	uint8_t povCount;
};

inline float intToFloat(int value)
{
	return (float)value;
}

inline int floatToInt(float value)
{
	return round(value);
}

extern "C"
{
	extern const uint32_t dio_kNumSystems;
	extern const uint32_t solenoid_kNumDO7_0Elements;
	extern const uint32_t interrupt_kNumSystems;
	extern const uint32_t kSystemClockTicksPerMicrosecond;

	void* getPort(uint8_t pin);
	void* getPortWithModule(uint8_t module, uint8_t pin);
	const char* getHALErrorMessage(int32_t code);

	uint16_t getFPGAVersion(int32_t *status);
	uint32_t getFPGARevision(int32_t *status);
	uint32_t getFPGATime(int32_t *status);

	bool getFPGAButton(int32_t *status);

	int HALSetErrorData(const char *errors, int errorsLength, int wait_ms);

	int HALGetControlWord(HALControlWord *data);
	int HALGetAllianceStation(enum HALAllianceStationID *allianceStation);
	int HALGetJoystickAxes(uint8_t joystickNum, HALJoystickAxes *axes);
	int HALGetJoystickPOVs(uint8_t joystickNum, HALJoystickPOVs *povs);
	int HALGetJoystickButtons(uint8_t joystickNum, HALJoystickButtons *buttons);
	int HALGetJoystickDescriptor(uint8_t joystickNum, HALJoystickDescriptor *desc);
	int HALSetJoystickOutputs(uint8_t joystickNum, uint32_t outputs, uint16_t leftRumble, uint16_t rightRumble);
	int HALGetMatchTime(float *matchTime);

	void HALSetNewDataSem(MULTIWAIT_ID sem);
	
	bool HALGetSystemActive(int32_t *status);
	bool HALGetBrownedOut(int32_t *status);

	int HALInitialize(int mode = 0);
	void HALNetworkCommunicationObserveUserProgramStarting();
	void HALNetworkCommunicationObserveUserProgramDisabled();
	void HALNetworkCommunicationObserveUserProgramAutonomous();
	void HALNetworkCommunicationObserveUserProgramTeleop();
	void HALNetworkCommunicationObserveUserProgramTest();

	uint32_t HALReport(uint8_t resource, uint8_t instanceNumber, uint8_t context = 0,
			const char *feature = NULL);
}

// TODO: HACKS for now...
extern "C"
{

	void NumericArrayResize();
	void RTSetCleanupProc();
	void EDVR_CreateReference();
	void Occur();
}
