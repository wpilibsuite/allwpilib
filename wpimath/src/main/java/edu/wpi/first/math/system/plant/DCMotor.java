// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.system.plant;

import edu.wpi.first.math.system.plant.proto.DCMotorProto;
import edu.wpi.first.math.system.plant.struct.DCMotorStruct;
import edu.wpi.first.math.util.Units;
import edu.wpi.first.util.protobuf.ProtobufSerializable;
import edu.wpi.first.util.struct.StructSerializable;

/** Holds the constants for a DC motor. */
public class DCMotor implements ProtobufSerializable, StructSerializable {
  /** Voltage at which the motor constants were measured. */
  public final double nominalVoltageVolts;

  /** Torque when stalled. */
  public final double stallTorqueNewtonMeters;

  /** Current draw when stalled. */
  public final double stallCurrentAmps;

  /** Current draw under no load. */
  public final double freeCurrentAmps;

  /** Angular velocity under no load. */
  public final double freeSpeedRadPerSec;

  /** Motor internal resistance. */
  public final double rOhms;

  /** Motor velocity constant. */
  public final double KvRadPerSecPerVolt;

  /** Motor torque constant. */
  public final double KtNMPerAmp;

  /** DCMotor protobuf for serialization. */
  public static final DCMotorProto proto = new DCMotorProto();

  /** DCMotor struct for serialization. */
  public static final DCMotorStruct struct = new DCMotorStruct();

  /**
   * Constructs a DC motor.
   *
   * @param nominalVoltageVolts Voltage at which the motor constants were measured.
   * @param stallTorqueNewtonMeters Torque when stalled.
   * @param stallCurrentAmps Current draw when stalled.
   * @param freeCurrentAmps Current draw under no load.
   * @param freeSpeedRadPerSec Angular velocity under no load.
   * @param numMotors Number of motors in a gearbox.
   */
  public DCMotor(
      double nominalVoltageVolts,
      double stallTorqueNewtonMeters,
      double stallCurrentAmps,
      double freeCurrentAmps,
      double freeSpeedRadPerSec,
      int numMotors) {
    this.nominalVoltageVolts = nominalVoltageVolts;
    this.stallTorqueNewtonMeters = stallTorqueNewtonMeters * numMotors;
    this.stallCurrentAmps = stallCurrentAmps * numMotors;
    this.freeCurrentAmps = freeCurrentAmps * numMotors;
    this.freeSpeedRadPerSec = freeSpeedRadPerSec;

    this.rOhms = nominalVoltageVolts / this.stallCurrentAmps;
    this.KvRadPerSecPerVolt =
        freeSpeedRadPerSec / (nominalVoltageVolts - rOhms * this.freeCurrentAmps);
    this.KtNMPerAmp = this.stallTorqueNewtonMeters / this.stallCurrentAmps;
  }

  /**
   * Calculate current drawn by motor with given speed and input voltage.
   *
   * @param speedRadiansPerSec The current angular velocity of the motor.
   * @param voltageInputVolts The voltage being applied to the motor.
   * @return The estimated current.
   */
  public double getCurrent(double speedRadiansPerSec, double voltageInputVolts) {
    return -1.0 / KvRadPerSecPerVolt / rOhms * speedRadiansPerSec + 1.0 / rOhms * voltageInputVolts;
  }

  /**
   * Calculate current drawn by motor for a given torque.
   *
   * @param torqueNm The torque produced by the motor.
   * @return The current drawn by the motor.
   */
  public double getCurrent(double torqueNm) {
    return torqueNm / KtNMPerAmp;
  }

  /**
   * Calculate torque produced by the motor with a given current.
   *
   * @param currentAmpere The current drawn by the motor.
   * @return The torque output.
   */
  public double getTorque(double currentAmpere) {
    return currentAmpere * KtNMPerAmp;
  }

  /**
   * Calculate the voltage provided to the motor for a given torque and angular velocity.
   *
   * @param torqueNm The torque produced by the motor.
   * @param speedRadiansPerSec The current angular velocity of the motor.
   * @return The voltage of the motor.
   */
  public double getVoltage(double torqueNm, double speedRadiansPerSec) {
    return 1.0 / KvRadPerSecPerVolt * speedRadiansPerSec + 1.0 / KtNMPerAmp * rOhms * torqueNm;
  }

  /**
   * Calculates the angular speed produced by the motor at a given torque and input voltage.
   *
   * @param torqueNm The torque produced by the motor.
   * @param voltageInputVolts The voltage applied to the motor.
   * @return The angular speed of the motor.
   */
  public double getSpeed(double torqueNm, double voltageInputVolts) {
    return voltageInputVolts * KvRadPerSecPerVolt
        - 1.0 / KtNMPerAmp * torqueNm * rOhms * KvRadPerSecPerVolt;
  }

  /**
   * Returns a copy of this motor with the given gearbox reduction applied.
   *
   * @param gearboxReduction The gearbox reduction.
   * @return A motor with the gearbox reduction applied.
   */
  public DCMotor withReduction(double gearboxReduction) {
    return new DCMotor(
        nominalVoltageVolts,
        stallTorqueNewtonMeters * gearboxReduction,
        stallCurrentAmps,
        freeCurrentAmps,
        freeSpeedRadPerSec / gearboxReduction,
        1);
  }

  /**
   * Return a gearbox of CIM motors.
   *
   * @param numMotors Number of motors in the gearbox.
   * @return A gearbox of CIM motors.
   */
  public static DCMotor getCIM(int numMotors) {
    return new DCMotor(
        12, 2.42, 133, 2.7, Units.rotationsPerMinuteToRadiansPerSecond(5310), numMotors);
  }

  /**
   * Return a gearbox of 775Pro motors.
   *
   * @param numMotors Number of motors in the gearbox.
   * @return A gearbox of 775Pro motors.
   */
  public static DCMotor getVex775Pro(int numMotors) {
    return new DCMotor(
        12, 0.71, 134, 0.7, Units.rotationsPerMinuteToRadiansPerSecond(18730), numMotors);
  }

  /**
   * Return a gearbox of NEO motors.
   *
   * @param numMotors Number of motors in the gearbox.
   * @return A gearbox of NEO motors.
   */
  public static DCMotor getNEO(int numMotors) {
    return new DCMotor(
        12, 2.6, 105, 1.8, Units.rotationsPerMinuteToRadiansPerSecond(5676), numMotors);
  }

  /**
   * Return a gearbox of MiniCIM motors.
   *
   * @param numMotors Number of motors in the gearbox.
   * @return A gearbox of MiniCIM motors.
   */
  public static DCMotor getMiniCIM(int numMotors) {
    return new DCMotor(
        12, 1.41, 89, 3, Units.rotationsPerMinuteToRadiansPerSecond(5840), numMotors);
  }

  /**
   * Return a gearbox of Bag motors.
   *
   * @param numMotors Number of motors in the gearbox.
   * @return A gearbox of Bag motors.
   */
  public static DCMotor getBag(int numMotors) {
    return new DCMotor(
        12, 0.43, 53, 1.8, Units.rotationsPerMinuteToRadiansPerSecond(13180), numMotors);
  }

  /**
   * Return a gearbox of Andymark RS775-125 motors.
   *
   * @param numMotors Number of motors in the gearbox.
   * @return A gearbox of Andymark RS775-125 motors.
   */
  public static DCMotor getAndymarkRs775_125(int numMotors) {
    return new DCMotor(
        12, 0.28, 18, 1.6, Units.rotationsPerMinuteToRadiansPerSecond(5800.0), numMotors);
  }

  /**
   * Return a gearbox of Banebots RS775 motors.
   *
   * @param numMotors Number of motors in the gearbox.
   * @return A gearbox of Banebots RS775 motors.
   */
  public static DCMotor getBanebotsRs775(int numMotors) {
    return new DCMotor(
        12, 0.72, 97, 2.7, Units.rotationsPerMinuteToRadiansPerSecond(13050.0), numMotors);
  }

  /**
   * Return a gearbox of Andymark 9015 motors.
   *
   * @param numMotors Number of motors in the gearbox.
   * @return A gearbox of Andymark 9015 motors.
   */
  public static DCMotor getAndymark9015(int numMotors) {
    return new DCMotor(
        12, 0.36, 71, 3.7, Units.rotationsPerMinuteToRadiansPerSecond(14270.0), numMotors);
  }

  /**
   * Return a gearbox of Banebots RS 550 motors.
   *
   * @param numMotors Number of motors in the gearbox.
   * @return A gearbox of Banebots RS 550 motors.
   */
  public static DCMotor getBanebotsRs550(int numMotors) {
    return new DCMotor(
        12, 0.38, 84, 0.4, Units.rotationsPerMinuteToRadiansPerSecond(19000.0), numMotors);
  }

  /**
   * Return a gearbox of NEO 550 motors.
   *
   * @param numMotors Number of motors in the gearbox.
   * @return A gearbox of NEO 550 motors.
   */
  public static DCMotor getNeo550(int numMotors) {
    return new DCMotor(
        12, 0.97, 100, 1.4, Units.rotationsPerMinuteToRadiansPerSecond(11000.0), numMotors);
  }

  /**
   * Return a gearbox of Falcon 500 motors.
   *
   * @param numMotors Number of motors in the gearbox.
   * @return A gearbox of Falcon 500 motors.
   */
  public static DCMotor getFalcon500(int numMotors) {
    return new DCMotor(
        12, 4.69, 257, 1.5, Units.rotationsPerMinuteToRadiansPerSecond(6380.0), numMotors);
  }

  /**
   * Return a gearbox of Falcon 500 motors with FOC (Field-Oriented Control) enabled.
   *
   * @param numMotors Number of motors in the gearbox.
   * @return A gearbox of Falcon 500 FOC enabled motors.
   */
  public static DCMotor getFalcon500Foc(int numMotors) {
    // https://store.ctr-electronics.com/falcon-500-powered-by-talon-fx/
    return new DCMotor(
        12, 5.84, 304, 1.5, Units.rotationsPerMinuteToRadiansPerSecond(6080.0), numMotors);
  }

  /**
   * Return a gearbox of Romi/TI_RSLK MAX motors.
   *
   * @param numMotors Number of motors in the gearbox.
   * @return A gearbox of Romi/TI_RSLK MAX motors.
   */
  public static DCMotor getRomiBuiltIn(int numMotors) {
    // From https://www.pololu.com/product/1520/specs
    return new DCMotor(
        4.5, 0.1765, 1.25, 0.13, Units.rotationsPerMinuteToRadiansPerSecond(150.0), numMotors);
  }

  /**
   * Return a gearbox of Kraken X60 brushless motors.
   *
   * @param numMotors Number of motors in the gearbox.
   * @return a gearbox of Kraken X60 motors.
   */
  public static DCMotor getKrakenX60(int numMotors) {
    // From https://store.ctr-electronics.com/announcing-kraken-x60/
    return new DCMotor(
        12, 7.09, 366, 2, Units.rotationsPerMinuteToRadiansPerSecond(6000), numMotors);
  }

  /**
   * Return a gearbox of Kraken X60 brushless motors with FOC (Field-Oriented Control) enabled.
   *
   * @param numMotors Number of motors in the gearbox.
   * @return A gearbox of Kraken X60 FOC enabled motors.
   */
  public static DCMotor getKrakenX60Foc(int numMotors) {
    // From https://store.ctr-electronics.com/announcing-kraken-x60/
    return new DCMotor(
        12, 9.37, 483, 2, Units.rotationsPerMinuteToRadiansPerSecond(5800), numMotors);
  }

  /**
   * Return a gearbox of Neo Vortex brushless motors.
   *
   * @param numMotors Number of motors in the gearbox.
   * @return a gearbox of Neo Vortex motors.
   */
  public static DCMotor getNeoVortex(int numMotors) {
    // From https://www.revrobotics.com/next-generation-spark-neo/
    return new DCMotor(
        12, 3.60, 211, 3.6, Units.rotationsPerMinuteToRadiansPerSecond(6784), numMotors);
  }
}
