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
   * MISCELLANEOUS for custom or miscellaneous devices.
   *
   * @see <a href=
   *     "https://docs.wpilib.org/en/stable/docs/software/can-devices/can-addressing.html">CAN
   *     Device Types</a>
   */
  public enum CANDeviceType {
    /** Broadcast. */
    BROADCAST(0),
    /** Robot controller. */
    ROBOT_CONTROLLER(1),
    /** Motor controller. */
    MOTOR_CONTROLLER(2),
    /** Relay controller. */
    RELAY_CONTROLLER(3),
    /** Gyro sensor. */
    GYRO_SENSOR(4),
    /** Accelerometer. */
    ACCELEROMETER(5),
    /** Ultrasonic sensor. */
    ULTRASONIC_SENSOR(6),
    /** Gear tooth sensor. */
    GEAR_TOOTH_SENSOR(7),
    /** Power distribution. */
    POWER_DISTRIBUTION(8),
    /** Pneumatics. */
    PNEUMATICS(9),
    /** Miscellaneous. */
    MISCELLANEOUS(10),
    /** IO breakout. */
    IO_BREAKOUT(11),
    /** Servo Controller. */
    SERVO_CONTROLLER(12),
    /** Firmware update. */
    FIRMWARE_UPDATE(31);

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
   * the TEAM_USE manufacturer ID for custom or team-specific devices.
   *
   * @see <a href=
   *     "https://docs.wpilib.org/en/stable/docs/software/can-devices/can-addressing.html">CAN
   *     Manufacturer IDs</a>
   */
  public enum CANManufacturer {
    /** Broadcast. */
    BROADCAST(0),
    /** National Instruments. */
    NI(1),
    /** Luminary Micro. */
    LM(2),
    /** DEKA Research and Development Corp. */
    DEKA(3),
    /** Cross the Road Electronics. */
    CTRE(4),
    /** REV Robotics. */
    REV(5),
    /** Grapple. */
    GRAPPLE(6),
    /** MindSensors. */
    MS(7),
    /** Team use. */
    TEAM_USE(8),
    /** Kauai Labs. */
    KAUAI_LABS(9),
    /** Copperforge. */
    COPPER_FORGE(10),
    /** Playing With Fusion. */
    PWF(11),
    /** Studica. */
    STUDICA(12),
    /** TheThriftyBot. */
    THE_THRIFTY_BOT(13),
    /** Redux Robotics. */
    REDUX_ROBOTICS(14),
    /** AndyMark. */
    ANDYMARK(15),
    /** Vivid-Hosting. */
    VIVID_HOSTING(16);

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
