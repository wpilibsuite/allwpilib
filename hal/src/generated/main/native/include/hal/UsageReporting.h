
#ifndef __UsageReporting_h__
#define __UsageReporting_h__

#ifdef _WIN32
#include <stdint.h>
#define EXPORT_FUNC __declspec(dllexport) __cdecl
#elif defined(__vxworks)
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
    kResourceType_Controller = 0,
    kResourceType_Module = 1,
    kResourceType_Language = 2,
    kResourceType_CANPlugin = 3,
    kResourceType_Accelerometer = 4,
    kResourceType_ADXL345 = 5,
    kResourceType_AnalogChannel = 6,
    kResourceType_AnalogTrigger = 7,
    kResourceType_AnalogTriggerOutput = 8,
    kResourceType_CANJaguar = 9,
    kResourceType_Compressor = 10,
    kResourceType_Counter = 11,
    kResourceType_Dashboard = 12,
    kResourceType_DigitalInput = 13,
    kResourceType_DigitalOutput = 14,
    kResourceType_DriverStationCIO = 15,
    kResourceType_DriverStationEIO = 16,
    kResourceType_DriverStationLCD = 17,
    kResourceType_Encoder = 18,
    kResourceType_GearTooth = 19,
    kResourceType_Gyro = 20,
    kResourceType_I2C = 21,
    kResourceType_Framework = 22,
    kResourceType_Jaguar = 23,
    kResourceType_Joystick = 24,
    kResourceType_Kinect = 25,
    kResourceType_KinectStick = 26,
    kResourceType_PIDController = 27,
    kResourceType_Preferences = 28,
    kResourceType_PWM = 29,
    kResourceType_Relay = 30,
    kResourceType_RobotDrive = 31,
    kResourceType_SerialPort = 32,
    kResourceType_Servo = 33,
    kResourceType_Solenoid = 34,
    kResourceType_SPI = 35,
    kResourceType_Task = 36,
    kResourceType_Ultrasonic = 37,
    kResourceType_Victor = 38,
    kResourceType_Button = 39,
    kResourceType_Command = 40,
    kResourceType_AxisCamera = 41,
    kResourceType_PCVideoServer = 42,
    kResourceType_SmartDashboard = 43,
    kResourceType_Talon = 44,
    kResourceType_HiTechnicColorSensor = 45,
    kResourceType_HiTechnicAccel = 46,
    kResourceType_HiTechnicCompass = 47,
    kResourceType_SRF08 = 48,
    kResourceType_AnalogOutput = 49,
    kResourceType_VictorSP = 50,
    kResourceType_PWMTalonSRX = 51,
    kResourceType_CANTalonSRX = 52,
    kResourceType_ADXL362 = 53,
    kResourceType_ADXRS450 = 54,
    kResourceType_RevSPARK = 55,
    kResourceType_MindsensorsSD540 = 56,
    kResourceType_DigitalGlitchFilter = 57,
    kResourceType_ADIS16448 = 58,
    kResourceType_PDP = 59,
    kResourceType_PCM = 60,
    kResourceType_PigeonIMU = 61,
    kResourceType_NidecBrushless = 62,
    kResourceType_CANifier = 63,
    kResourceType_TalonFX = 64,
    kResourceType_CTRE_future1 = 65,
    kResourceType_CTRE_future2 = 66,
    kResourceType_CTRE_future3 = 67,
    kResourceType_CTRE_future4 = 68,
    kResourceType_CTRE_future5 = 69,
    kResourceType_CTRE_future6 = 70,
    kResourceType_LinearFilter = 71,
    kResourceType_XboxController = 72,
    kResourceType_UsbCamera = 73,
    kResourceType_NavX = 74,
    kResourceType_Pixy = 75,
    kResourceType_Pixy2 = 76,
    kResourceType_ScanseSweep = 77,
    kResourceType_Shuffleboard = 78,
    kResourceType_CAN = 79,
    kResourceType_DigilentDMC60 = 80,
    kResourceType_PWMVictorSPX = 81,
    kResourceType_RevSparkMaxPWM = 82,
    kResourceType_RevSparkMaxCAN = 83,
    kResourceType_ADIS16470 = 84,
    kResourceType_PIDController2 = 85,
    kResourceType_ProfiledPIDController = 86,
    kResourceType_Kinematics = 87,
    kResourceType_Odometry = 88,
    kResourceType_Units = 89,
    kResourceType_TrapezoidProfile = 90,
    kResourceType_DutyCycle = 91,
    kResourceType_AddressableLEDs = 92,
    kResourceType_FusionVenom = 93,
    kResourceType_CTRE_future7 = 94,
    kResourceType_CTRE_future8 = 95,
    kResourceType_CTRE_future9 = 96,
    kResourceType_CTRE_future10 = 97,
    kResourceType_CTRE_future11 = 98,
    kResourceType_CTRE_future12 = 99,
    kResourceType_CTRE_future13 = 100,
    kResourceType_CTRE_future14 = 101,
    kResourceType_ExponentialProfile = 102,
    kResourceType_PS4Controller = 103,
    kResourceType_PhotonCamera = 104,
    kResourceType_PhotonPoseEstimator = 105,
    kResourceType_PathPlannerPath = 106,
    kResourceType_PathPlannerAuto = 107,
    kResourceType_PathFindingCommand = 108,
    kResourceType_Redux_future1 = 109,
    kResourceType_Redux_future2 = 110,
    kResourceType_Redux_future3 = 111,
    kResourceType_Redux_future4 = 112,
    kResourceType_Redux_future5 = 113,
    kResourceType_RevSparkFlexCAN = 114,
    kResourceType_RevSparkFlexPWM = 115,
    kResourceType_BangBangController = 116,
    kResourceType_DataLogManager = 117,
    kResourceType_LoggingFramework = 118,
    kResourceType_ChoreoTrajectory = 119,
    kResourceType_ChoreoTrigger = 120,
    kResourceType_PathWeaverTrajectory = 121,
    kResourceType_Koors40 = 122,
    kResourceType_ThriftyNova = 123,
    kResourceType_RevServoHub = 124,
    kResourceType_PWFSEN36005 = 125,
    kResourceType_LaserShark = 126,

//    kResourceType_MaximumID = 255,
} tResourceType;

typedef enum
{
    kLanguage_LabVIEW = 1,
    kLanguage_CPlusPlus = 2,
    kLanguage_Java = 3,
    kLanguage_Python = 4,
    kLanguage_DotNet = 5,
    kLanguage_Kotlin = 6,
    kLanguage_Rust = 7,
    kCANPlugin_BlackJagBridge = 1,
    kCANPlugin_2CAN = 2,
    kFramework_Iterative = 1,
    kFramework_Simple = 2,
    kFramework_CommandControl = 3,
    kFramework_Timed = 4,
    kFramework_ROS = 5,
    kFramework_RobotBuilder = 6,
    kFramework_AdvantageKit = 7,
    kFramework_MagicBot = 8,
    kFramework_KitBotTraditional = 9,
    kFramework_KitBotInline = 10,
    kRobotDrive_ArcadeStandard = 1,
    kRobotDrive_ArcadeButtonSpin = 2,
    kRobotDrive_ArcadeRatioCurve = 3,
    kRobotDrive_Tank = 4,
    kRobotDrive_MecanumPolar = 5,
    kRobotDrive_MecanumCartesian = 6,
    kRobotDrive2_DifferentialArcade = 7,
    kRobotDrive2_DifferentialTank = 8,
    kRobotDrive2_DifferentialCurvature = 9,
    kRobotDrive2_MecanumCartesian = 10,
    kRobotDrive2_MecanumPolar = 11,
    kRobotDrive2_KilloughCartesian = 12,
    kRobotDrive2_KilloughPolar = 13,
    kRobotDriveSwerve_Other = 14,
    kRobotDriveSwerve_YAGSL = 15,
    kRobotDriveSwerve_CTRE = 16,
    kRobotDriveSwerve_MaxSwerve = 17,
    kRobotDriveSwerve_AdvantageKit = 18,
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
    kCommand2_Scheduler = 2,
    kSmartDashboard_Instance = 1,
    kSmartDashboard_LiveWindow = 2,
    kKinematics_DifferentialDrive = 1,
    kKinematics_MecanumDrive = 2,
    kKinematics_SwerveDrive = 3,
    kOdometry_DifferentialDrive = 1,
    kOdometry_MecanumDrive = 2,
    kOdometry_SwerveDrive = 3,
    kDashboard_Unknown = 1,
    kDashboard_Glass = 2,
    kDashboard_SmartDashboard = 3,
    kDashboard_Shuffleboard = 4,
    kDashboard_Elastic = 5,
    kDashboard_LabVIEW = 6,
    kDashboard_AdvantageScope = 7,
    kDashboard_QFRCDashboard = 8,
    kDashboard_FRCWebComponents = 9,
    kDataLogLocation_Onboard = 1,
    kDataLogLocation_USB = 2,
    kLoggingFramework_Other = 1,
    kLoggingFramework_Epilogue = 2,
    kLoggingFramework_Monologue = 3,
    kLoggingFramework_AdvantageKit = 4,
    kLoggingFramework_DogLog = 5,
    kPDP_CTRE = 1,
    kPDP_REV = 2,
    kPDP_Unknown = 3,
} tInstances;

/**
 * Report the usage of a resource of interest.
 *
 * @param resource one of the values in the tResourceType above (max value 51).
 * @param instanceNumber an index that identifies the resource instance.
 * @param context an optional additional context number for some cases (such as module number).  Set to 0 to omit.
 * @param feature a string to be included describing features in use on a specific resource.  Setting the same resource more than once allows you to change the feature string.
 */
uint32_t EXPORT_FUNC report(tResourceType resource, uint8_t instanceNumber, uint8_t context = 0, const char* feature = NULL);
} // namespace nUsageReporting

#ifdef __cplusplus
extern "C"
{
#endif

    uint32_t EXPORT_FUNC FRC_NetworkCommunication_nUsageReporting_report(uint8_t resource, uint8_t instanceNumber, uint8_t context, const char* feature);

#ifdef __cplusplus
}
#endif

#endif // __UsageReporting_h__
