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
public class CANAPITypes {
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
    /** kBroadcast = 0. */
    kBroadcast(0),
    /** kRobotController = 1. */
    kRobotController(1),
    /** kMotorController = 2. */
    kMotorController(2),
    /** kRelayController = 3. */
    kRelayController(3),
    /** kGyroSensor = 4. */
    kGyroSensor(4),
    /** kAccelerometer = 5. */
    kAccelerometer(5),
    /** kUltrasonicSensor = 6. */
    kUltrasonicSensor(6),
    /** kGearToothSensor = 7. */
    kGearToothSensor(7),
    /** kPowerDistribution = 8. */
    kPowerDistribution(8),
    /** kPneumatics = 9. */
    kPneumatics(9),
    /** kMiscellaneous = 10. */
    kMiscellaneous(10),
    /** kIOBreakout = 11. */
    kIOBreakout(11),
    /** kFirmwareUpdate = 31. */
    kFirmwareUpdate(31);

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
    /** kBroadcast = 0. */
    kBroadcast(0),
    /** National Instruments = 1. */
    kNI(1),
    /** Luminary Micro = 2. */
    kLM(2),
    /** DEKA Research and Development Corp = 3. */
    kDEKA(3),
    /** Cross the Road Electronics = 4. */
    kCTRE(4),
    /** REV robotics = 5. */
    kREV(5),
    /** kGrapple = 6. */
    kGrapple(6),
    /** MindSensors = 7. */
    kMS(7),
    /** kTeamUse = 8. */
    kTeamUse(8),
    /** kKauaiLabs = 9. */
    kKauaiLabs(9),
    /** kCopperforge = 10. */
    kCopperforge(10),
    /** Playing With Fusion = 11. */
    kPWF(11),
    /** kStudica = 12. */
    kStudica(12),
    /** kTheThriftyBot = 13. */
    kTheThriftyBot(13),
    /** kReduxRobotics = 14. */
    kReduxRobotics(14),
    /** kAndyMark = 15. */
    kAndyMark(15),
    /** kVividHosting = 16. */
    kVividHosting(16);

    @SuppressWarnings("PMD.MemberName")
    public final int id;

    CANManufacturer(int id) {
      this.id = id;
    }
  }
}
