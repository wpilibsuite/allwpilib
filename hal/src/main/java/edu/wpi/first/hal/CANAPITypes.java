// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/**
 * CAN API Types.
 *
 * <p>This class defines enums for CAN device types and manufacturer IDs as specified in the WPILib
 * documentation: https://docs.wpilib.org/en/stable/docs/software/can-devices/can-addressing.html
 */
@SuppressWarnings("PMD.MissingStaticMethodInNonInstantiatableClass")
public final class CANAPITypes {
  /**
   * FRC CAN device type.
   *
   * <p>This enum represents different types of CAN devices. Teams are encouraged to use the
   * kMiscellaneous for custom or miscellaneous devices.
   *
   * @see <a href=
   *     "https://docs.wpilib.org/en/stable/docs/software/can-devices/can-addressing.html">CAN
   *     Device Types</a>
   */
  public enum CANDeviceType {
    /** Broadcast. */
    kBroadcast(0),
    /** Robot controller. */
    kRobotController(1),
    /** Motor controller. */
    kMotorController(2),
    /** Relay controller. */
    kRelayController(3),
    /** Gyro sensor. */
    kGyroSensor(4),
    /** Accelerometer. */
    kAccelerometer(5),
    /** Ultrasonic sensor. */
    kUltrasonicSensor(6),
    /** Gear tooth sensor. */
    kGearToothSensor(7),
    /** Power distribution. */
    kPowerDistribution(8),
    /** Pneumatics. */
    kPneumatics(9),
    /** Miscellaneous. */
    kMiscellaneous(10),
    /** IO breakout. */
    kIOBreakout(11),
    /** Servo Controller. */
    kServoController(12),
    /** Firmware update. */
    kFirmwareUpdate(31);

    /** The device type ID. */
    @SuppressWarnings("PMD.MemberName")
    public final int id;

    CANDeviceType(int id) {
      this.id = id;
    }
  }

  /**
   * FRC CAN manufacturer ID.
   *
   * <p>This enum represents different manufacturer IDs for CAN devices. Teams are encouraged to use
   * the kTeamUse manufacturer ID for custom or team-specific devices.
   *
   * @see <a href=
   *     "https://docs.wpilib.org/en/stable/docs/software/can-devices/can-addressing.html">CAN
   *     Manufacturer IDs</a>
   */
  public enum CANManufacturer {
    /** Broadcast. */
    kBroadcast(0),
    /** National Instruments. */
    kNI(1),
    /** Luminary Micro. */
    kLM(2),
    /** DEKA Research and Development Corp. */
    kDEKA(3),
    /** Cross the Road Electronics. */
    kCTRE(4),
    /** REV Robotics. */
    kREV(5),
    /** Grapple. */
    kGrapple(6),
    /** MindSensors. */
    kMS(7),
    /** Team use. */
    kTeamUse(8),
    /** Kauai Labs. */
    kKauaiLabs(9),
    /** Copperforge. */
    kCopperforge(10),
    /** Playing With Fusion. */
    kPWF(11),
    /** Studica. */
    kStudica(12),
    /** TheThriftyBot. */
    kTheThriftyBot(13),
    /** Redux Robotics. */
    kReduxRobotics(14),
    /** AndyMark. */
    kAndyMark(15),
    /** Vivid-Hosting. */
    kVividHosting(16);

    /** The manufacturer ID. */
    @SuppressWarnings("PMD.MemberName")
    public final int id;

    CANManufacturer(int id) {
      this.id = id;
    }
  }

  /** Utility class. */
  private CANAPITypes() {}
}
