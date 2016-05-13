/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.communication;

import java.nio.ByteBuffer;

import edu.wpi.first.wpilibj.hal.JNIWrapper;

/**
 * JNI Wrapper for library <b>FRC_NetworkCommunications</b><br>.
 */
@SuppressWarnings("MethodName")
public class FRCNetworkCommunicationsLibrary extends JNIWrapper {
  /**
   * Module type from LoadOut.h
   */
  @SuppressWarnings("TypeName")
  public interface tModuleType {
    int kModuleType_Unknown = 0x00;
    int kModuleType_Analog = 0x01;
    int kModuleType_Digital = 0x02;
    int kModuleType_Solenoid = 0x03;
  }

  /**
   * Target class from LoadOut.h
   */
  @SuppressWarnings("TypeName")
  public interface tTargetClass {
    int kTargetClass_Unknown = 0x00;
    int kTargetClass_FRC1 = 0x10;
    int kTargetClass_FRC2 = 0x20;
    int kTargetClass_FRC2_Analog =
        kTargetClass_FRC2 | FRCNetworkCommunicationsLibrary.tModuleType.kModuleType_Analog;
    int kTargetClass_FRC2_Digital =
        kTargetClass_FRC2 | FRCNetworkCommunicationsLibrary.tModuleType.kModuleType_Digital;
    int kTargetClass_FRC2_Solenoid =
        kTargetClass_FRC2 | FRCNetworkCommunicationsLibrary.tModuleType.kModuleType_Solenoid;
    int kTargetClass_FamilyMask = 0xF0;
    int kTargetClass_ModuleMask = 0x0F;
  }

  /**
   * Resource types from UsageReporting.h
   */
  @SuppressWarnings("TypeName")
  public interface tResourceType {
    int kResourceType_Controller = 0;
    int kResourceType_Module = 1;
    int kResourceType_Language = 2;
    int kResourceType_CANPlugin = 3;
    int kResourceType_Accelerometer = 4;
    int kResourceType_ADXL345 = 5;
    int kResourceType_AnalogChannel = 6;
    int kResourceType_AnalogTrigger = 7;
    int kResourceType_AnalogTriggerOutput = 8;
    int kResourceType_CANJaguar = 9;
    int kResourceType_Compressor = 10;
    int kResourceType_Counter = 11;
    int kResourceType_Dashboard = 12;
    int kResourceType_DigitalInput = 13;
    int kResourceType_DigitalOutput = 14;
    int kResourceType_DriverStationCIO = 15;
    int kResourceType_DriverStationEIO = 16;
    int kResourceType_DriverStationLCD = 17;
    int kResourceType_Encoder = 18;
    int kResourceType_GearTooth = 19;
    int kResourceType_Gyro = 20;
    int kResourceType_I2C = 21;
    int kResourceType_Framework = 22;
    int kResourceType_Jaguar = 23;
    int kResourceType_Joystick = 24;
    int kResourceType_Kinect = 25;
    int kResourceType_KinectStick = 26;
    int kResourceType_PIDController = 27;
    int kResourceType_Preferences = 28;
    int kResourceType_PWM = 29;
    int kResourceType_Relay = 30;
    int kResourceType_RobotDrive = 31;
    int kResourceType_SerialPort = 32;
    int kResourceType_Servo = 33;
    int kResourceType_Solenoid = 34;
    int kResourceType_SPI = 35;
    int kResourceType_Task = 36;
    int kResourceType_Ultrasonic = 37;
    int kResourceType_Victor = 38;
    int kResourceType_Button = 39;
    int kResourceType_Command = 40;
    int kResourceType_AxisCamera = 41;
    int kResourceType_PCVideoServer = 42;
    int kResourceType_SmartDashboard = 43;
    int kResourceType_Talon = 44;
    int kResourceType_HiTechnicColorSensor = 45;
    int kResourceType_HiTechnicAccel = 46;
    int kResourceType_HiTechnicCompass = 47;
    int kResourceType_SRF08 = 48;
    int kResourceType_AnalogOutput = 49;
    int kResourceType_VictorSP = 50;
    int kResourceType_TalonSRX = 51;
    int kResourceType_CANTalonSRX = 52;
    int kResourceType_ADXL362 = 53;
    int kResourceType_ADXRS450 = 54;
    int kResourceType_RevSPARK = 55;
    int kResourceType_MindsensorsSD540 = 56;
    int kResourceType_DigitalFilter = 57;
  }

  /**
   * Instances from UsageReporting.h
   */
  @SuppressWarnings("TypeName")
  public interface tInstances {
    int kLanguage_LabVIEW = 1;
    int kLanguage_CPlusPlus = 2;
    int kLanguage_Java = 3;
    int kLanguage_Python = 4;

    int kCANPlugin_BlackJagBridge = 1;
    int kCANPlugin_2CAN = 2;

    int kFramework_Iterative = 1;
    int kFramework_Sample = 2;
    int kFramework_CommandControl = 3;

    int kRobotDrive_ArcadeStandard = 1;
    int kRobotDrive_ArcadeButtonSpin = 2;
    int kRobotDrive_ArcadeRatioCurve = 3;
    int kRobotDrive_Tank = 4;
    int kRobotDrive_MecanumPolar = 5;
    int kRobotDrive_MecanumCartesian = 6;

    int kDriverStationCIO_Analog = 1;
    int kDriverStationCIO_DigitalIn = 2;
    int kDriverStationCIO_DigitalOut = 3;

    int kDriverStationEIO_Acceleration = 1;
    int kDriverStationEIO_AnalogIn = 2;
    int kDriverStationEIO_AnalogOut = 3;
    int kDriverStationEIO_Button = 4;
    int kDriverStationEIO_LED = 5;
    int kDriverStationEIO_DigitalIn = 6;
    int kDriverStationEIO_DigitalOut = 7;
    int kDriverStationEIO_FixedDigitalOut = 8;
    int kDriverStationEIO_PWM = 9;
    int kDriverStationEIO_Encoder = 10;
    int kDriverStationEIO_TouchSlider = 11;

    int kADXL345_SPI = 1;
    int kADXL345_I2C = 2;

    int kCommand_Scheduler = 1;

    int kSmartDashboard_Instance = 1;
  }

  /**
   * Report the usage of a resource of interest. <br>
   *
   * <p>Original signature: <code>uint32_t report(tResourceType, uint8_t, uint8_t, const
   * char*)</code>
   *
   * @param resource       one of the values in the tResourceType above (max value 51). <br>
   * @param instanceNumber an index that identifies the resource instance. <br>
   * @param context        an optional additional context number for some cases (such as module
   *                       number). Set to 0 to omit. <br>
   * @param feature        a string to be included describing features in use on a specific
   *                       resource. Setting the same resource more than once allows you to change
   *                       the feature string.
   */
  public static native int FRCNetworkCommunicationUsageReportingReport(byte resource,
                                                                       byte instanceNumber,
                                                                       byte context,
                                                                       String feature);

  public static native void setNewDataSem(long mutexId);

  public static native void FRCNetworkCommunicationObserveUserProgramStarting();

  public static native void FRCNetworkCommunicationObserveUserProgramDisabled();

  public static native void FRCNetworkCommunicationObserveUserProgramAutonomous();

  public static native void FRCNetworkCommunicationObserveUserProgramTeleop();

  public static native void FRCNetworkCommunicationObserveUserProgramTest();

  public static native void FRCNetworkCommunicationReserve();

  private static native int NativeHALGetControlWord();

  @SuppressWarnings("JavadocMethod")
  public static HALControlWord HALGetControlWord() {
    int word = NativeHALGetControlWord();
    return new HALControlWord((word & 1) != 0, ((word >> 1) & 1) != 0, ((word >> 2) & 1) != 0,
        ((word >> 3) & 1) != 0, ((word >> 4) & 1) != 0, ((word >> 5) & 1) != 0);
  }

  private static native int NativeHALGetAllianceStation();

  @SuppressWarnings("JavadocMethod")
  public static HALAllianceStationID HALGetAllianceStation() {
    switch (NativeHALGetAllianceStation()) {
      case 0:
        return HALAllianceStationID.Red1;
      case 1:
        return HALAllianceStationID.Red2;
      case 2:
        return HALAllianceStationID.Red3;
      case 3:
        return HALAllianceStationID.Blue1;
      case 4:
        return HALAllianceStationID.Blue2;
      case 5:
        return HALAllianceStationID.Blue3;
      default:
        return null;
    }
  }

  public static int kMaxJoystickAxes = 12;
  public static int kMaxJoystickPOVs = 12;

  public static native short[] HALGetJoystickAxes(byte joystickNum);

  public static native short[] HALGetJoystickPOVs(byte joystickNum);

  public static native int HALGetJoystickButtons(byte joystickNum, ByteBuffer count);

  public static native int HALSetJoystickOutputs(byte joystickNum, int outputs, short leftRumble,
                                                 short rightRumble);

  public static native int HALGetJoystickIsXbox(byte joystickNum);

  public static native int HALGetJoystickType(byte joystickNum);

  public static native String HALGetJoystickName(byte joystickNum);

  public static native int HALGetJoystickAxisType(byte joystickNum, byte axis);

  public static native float HALGetMatchTime();

  public static native boolean HALGetSystemActive();

  public static native boolean HALGetBrownedOut();

  public static native int HALSetErrorData(String error);

  public static native int HALSendError(boolean isError, int errorCode, boolean isLVCode,
                                        String details, String location, String callStack,
                                        boolean printMsg);
}
