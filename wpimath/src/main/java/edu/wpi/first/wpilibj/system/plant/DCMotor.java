/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.system.plant;

import edu.wpi.first.wpilibj.util.Units;

/**
 * Holds the constants for a DC motor.
 */
public class DCMotor {
  public final double m_nominalVoltageVolts;
  public final double m_stallTorqueNewtonMeters;
  public final double m_stallCurrentAmps;
  public final double m_freeCurrentAmps;
  public final double m_freeSpeedRadPerSec;
  @SuppressWarnings("MemberName")
  public final double m_rOhms;
  @SuppressWarnings("MemberName")
  public final double m_KvRadPerSecPerVolt;
  @SuppressWarnings("MemberName")
  public final double m_KtNMPerAmp;


  /**
   * Constructs a DC motor.
   *
   * @param nominalVoltageVolts     Voltage at which the motor constants were measured.
   * @param stallTorqueNewtonMeters Current draw when stalled.
   * @param stallCurrentAmps        Current draw when stalled.
   * @param freeCurrentAmps         Current draw under no load.
   * @param freeSpeedRadPerSec      Angular velocity under no load.
   */
  public DCMotor(double nominalVoltageVolts,
                 double stallTorqueNewtonMeters,
                 double stallCurrentAmps,
                 double freeCurrentAmps,
                 double freeSpeedRadPerSec) {
    this.m_nominalVoltageVolts = nominalVoltageVolts;
    this.m_stallTorqueNewtonMeters = stallTorqueNewtonMeters;
    this.m_stallCurrentAmps = stallCurrentAmps;
    this.m_freeCurrentAmps = freeCurrentAmps;
    this.m_freeSpeedRadPerSec = freeSpeedRadPerSec;

    this.m_rOhms = nominalVoltageVolts / stallCurrentAmps;
    this.m_KvRadPerSecPerVolt = freeSpeedRadPerSec / (nominalVoltageVolts - m_rOhms
      * freeCurrentAmps);
    this.m_KtNMPerAmp = stallTorqueNewtonMeters / stallCurrentAmps;
  }

  /**
   * Estimate the current being drawn by this motor.
   *
   * @param speedRadiansPerSec The speed of the rotor.
   * @param voltageInputVolts  The input voltage.
   */
  public double getCurrent(double speedRadiansPerSec, double voltageInputVolts) {
    return -1.0 / m_KvRadPerSecPerVolt / m_rOhms * speedRadiansPerSec
      + 1.0 / m_rOhms * voltageInputVolts;
  }

  /**
   * Return a gearbox of CIM motors.
   *
   * @param numMotors Number of motors in the gearbox.
   */
  public static DCMotor getCIM(int numMotors) {
    return new DCMotor(12,
      2.42 * numMotors, 133,
      2.7, Units.rotationsPerMinuteToRadiansPerSecond(5310));
  }

  /**
   * Return a gearbox of 775Pro motors.
   *
   * @param numMotors Number of motors in the gearbox.
   */
  public static DCMotor getVex775Pro(int numMotors) {
    return gearbox(new DCMotor(12,
      0.71, 134,
      0.7, Units.rotationsPerMinuteToRadiansPerSecond(18730)), numMotors);
  }

  /**
   * Return a gearbox of NEO motors.
   *
   * @param numMotors Number of motors in the gearbox.
   */
  public static DCMotor getNEO(int numMotors) {
    return gearbox(new DCMotor(12, 2.6,
      105, 1.8, Units.rotationsPerMinuteToRadiansPerSecond(5676)), numMotors);
  }

  /**
   * Return a gearbox of MiniCIM motors.
   *
   * @param numMotors Number of motors in the gearbox.
   */
  public static DCMotor getMiniCIM(int numMotors) {
    return gearbox(new DCMotor(12, 1.41, 89, 3,
      Units.rotationsPerMinuteToRadiansPerSecond(5840)), numMotors);
  }

  /**
   * Return a gearbox of Bag motors.
   *
   * @param numMotors Number of motors in the gearbox.
   */
  public static DCMotor getBag(int numMotors) {
    return gearbox(new DCMotor(12, 0.43, 53, 1.8,
      Units.rotationsPerMinuteToRadiansPerSecond(13180)), numMotors);
  }

  /**
   * Return a gearbox of Andymark RS775-125 motors.
   *
   * @param numMotors Number of motors in the gearbox.
   */
  public static DCMotor getAndymarkRs775_125(int numMotors) {
    return gearbox(new DCMotor(12, 0.28, 18, 1.6,
      Units.rotationsPerMinuteToRadiansPerSecond(5800.0)), numMotors);
  }

  /**
   * Return a gearbox of Banebots RS775 motors.
   *
   * @param numMotors Number of motors in the gearbox.
   */
  public static DCMotor getBanebotsRs775(int numMotors) {
    return gearbox(new DCMotor(12, 0.72, 97, 2.7,
      Units.rotationsPerMinuteToRadiansPerSecond(13050.0)), numMotors);
  }

  /**
   * Return a gearbox of Andymark 9015 motors.
   *
   * @param numMotors Number of motors in the gearbox.
   */
  public static DCMotor getAndymark9015(int numMotors) {
    return gearbox(new DCMotor(12, 0.36, 71, 3.7,
      Units.rotationsPerMinuteToRadiansPerSecond(14270.0)), numMotors);
  }

  /**
   * Return a gearbox of Banebots RS 550 motors.
   *
   * @param numMotors Number of motors in the gearbox.
   */
  public static DCMotor getBanebotsRs550(int numMotors) {
    return gearbox(new DCMotor(12, 0.38, 84, 0.4,
      Units.rotationsPerMinuteToRadiansPerSecond(19000.0)), numMotors);
  }

  /**
   * Return a gearbox of Neo 550 motors.
   *
   * @param numMotors Number of motors in the gearbox.
   */
  public static DCMotor getNeo550(int numMotors) {
    return gearbox(new DCMotor(12, 0.97, 100, 1.4,
      Units.rotationsPerMinuteToRadiansPerSecond(11000.0)), numMotors);
  }

  /**
   * Return a gearbox of Falcon 500 motors.
   *
   * @param numMotors Number of motors in the gearbox.
   */
  public static DCMotor getFalcon500(int numMotors) {
    return gearbox(new DCMotor(12, 4.69, 257, 1.5,
      Units.rotationsPerMinuteToRadiansPerSecond(6380.0)), numMotors);
  }

  private static DCMotor gearbox(DCMotor motor, double numMotors) {
    return new DCMotor(motor.m_nominalVoltageVolts, motor.m_stallTorqueNewtonMeters * numMotors,
      motor.m_stallCurrentAmps, motor.m_freeCurrentAmps, motor.m_freeSpeedRadPerSec);
  }
}
