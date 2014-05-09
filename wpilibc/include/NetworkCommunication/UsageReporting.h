
#ifndef __UsageReporting_h__
#define __UsageReporting_h__

#ifdef SIMULATION
#include <vxWorks_compat.h>
#define EXPORT_FUNC __declspec(dllexport) __cdecl
#elif defined (__vxworks)
#include <vxWorks.h>
#define EXPORT_FUNC
#else
#include <stdint.h>
#include <stdlib.h>
#define EXPORT_FUNC
#endif

#define kUsageReporting_version 1

namespace nUsageReporting
{
    typedef enum
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
    } tResourceType;

    typedef enum
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
    } tInstances;

    /**
     * Report the usage of a resource of interest.
     * 
     * @param resource one of the values in the tResourceType above (max value 51).
     * @param instanceNumber an index that identifies the resource instance.
     * @param context an optional additional context number for some cases (such as module number).  Set to 0 to omit.
     * @param feature a string to be included describing features in use on a specific resource.  Setting the same resource more than once allows you to change the feature string.
     */
    uint32_t EXPORT_FUNC report(tResourceType resource, uint8_t instanceNumber, uint8_t context = 0, const char *feature = NULL);
}

#ifdef __cplusplus
extern "C" {
#endif

    uint32_t EXPORT_FUNC FRC_NetworkCommunication_nUsageReporting_report(uint8_t resource, uint8_t instanceNumber, uint8_t context, const char *feature);

#ifdef __cplusplus
}
#endif

#endif // __UsageReporting_h__
