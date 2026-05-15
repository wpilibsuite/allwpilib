// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.system;

import org.wpilib.math.system.proto.DCMotorProto;
import org.wpilib.math.system.struct.DCMotorStruct;
import org.wpilib.math.util.Units;
import org.wpilib.util.protobuf.ProtobufSerializable;
import org.wpilib.util.struct.StructSerializable;

/** Holds the constants for a DC motor. */
public class DCMotor implements ProtobufSerializable, StructSerializable {
  /** Voltage at which the motor constants were measured. */
  public final double nominalVoltage;

  /** Torque when stalled in Newton-meters. */
  public final double stallTorque;

  /** Current draw when stalled in amps. */
  public final double stallCurrent;

  /** Current draw under no load in amps. */
  public final double freeCurrent;

  /** Angular velocity under no load in radians per second. */
  public final double freeSpeed;

  /** Motor internal resistance in Ohms. */
  public final double R;

  /** Motor velocity constant in (rad/s)/V. */
  public final double Kv;

  /** Motor torque constant in Newton-meters per amp. */
  public final double Kt;

  /** DCMotor protobuf for serialization. */
  public static final DCMotorProto proto = new DCMotorProto();

  /** DCMotor struct for serialization. */
  public static final DCMotorStruct struct = new DCMotorStruct();

  /**
   * Constructs a DC motor.
   *
   * @param nominalVoltage Voltage at which the motor constants were measured.
   * @param stallTorque Torque when stalled.
   * @param stallCurrent Current draw when stalled.
   * @param freeCurrent Current draw under no load.
   * @param freeSpeed Angular velocity under no load.
   * @param numMotors Number of motors in a gearbox.
   */
  public DCMotor(
      double nominalVoltage,
      double stallTorque,
      double stallCurrent,
      double freeCurrent,
      double freeSpeed,
      int numMotors) {
    this.nominalVoltage = nominalVoltage;
    this.stallTorque = stallTorque * numMotors;
    this.stallCurrent = stallCurrent * numMotors;
    this.freeCurrent = freeCurrent * numMotors;
    this.freeSpeed = freeSpeed;

    this.R = nominalVoltage / this.stallCurrent;
    this.Kv = freeSpeed / (nominalVoltage - R * this.freeCurrent);
    this.Kt = this.stallTorque / this.stallCurrent;
  }

  /**
   * Calculate current drawn by motor with given velocity and input voltage.
   *
   * @param velocity The current angular velocity of the motor.
   * @param voltageInput The voltage being applied to the motor.
   * @return The estimated current.
   */
  public double getCurrent(double velocity, double voltageInput) {
    return -1.0 / Kv / R * velocity + 1.0 / R * voltageInput;
  }

  /**
   * Calculate current drawn by motor for a given torque.
   *
   * @param torque The torque produced by the motor in Newton-meters.
   * @return The current drawn by the motor.
   */
  public double getCurrent(double torque) {
    return torque / Kt;
  }

  /**
   * Calculate torque produced by the motor with a given current.
   *
   * @param current The current drawn by the motor in amps.
   * @return The torque output in Newton-meters.
   */
  public double getTorque(double current) {
    return current * Kt;
  }

  /**
   * Calculate the voltage provided to the motor for a given torque and angular velocity.
   *
   * @param torque The torque produced by the motor in Newton-meters.
   * @param velocity The current angular velocity of the motor in radians per second.
   * @return The voltage of the motor.
   */
  public double getVoltage(double torque, double velocity) {
    return 1.0 / Kv * velocity + 1.0 / Kt * R * torque;
  }

  /**
   * Calculates the angular velocity produced by the motor at a given torque and input voltage.
   *
   * @param torque The torque produced by the motor in Newton-meters.
   * @param voltageInput The voltage applied to the motor.
   * @return The angular velocity of the motor.
   */
  public double getVelocity(double torque, double voltageInput) {
    return voltageInput * Kv - 1.0 / Kt * torque * R * Kv;
  }

  /**
   * Returns a copy of this motor with the given gearbox reduction applied.
   *
   * @param gearboxReduction The gearbox reduction.
   * @return A motor with the gearbox reduction applied.
   */
  public DCMotor withReduction(double gearboxReduction) {
    return new DCMotor(
        nominalVoltage,
        stallTorque * gearboxReduction,
        stallCurrent,
        freeCurrent,
        freeSpeed / gearboxReduction,
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
   * Return a gearbox of Kraken X44 brushless motors.
   *
   * @param numMotors Number of motors in the gearbox.
   * @return a gearbox of Kraken X44 motors.
   */
  public static DCMotor getKrakenX44(int numMotors) {
    // From https://motors.ctr-electronics.com/dyno/dynometer-testing/
    return new DCMotor(
        12, 4.11, 279, 2, Units.rotationsPerMinuteToRadiansPerSecond(7758), numMotors);
  }

  /**
   * Return a gearbox of Kraken X44 brushless motors with FOC (Field-Oriented Control) enabled.
   *
   * @param numMotors Number of motors in the gearbox.
   * @return A gearbox of Kraken X44 FOC enabled motors.
   */
  public static DCMotor getKrakenX44Foc(int numMotors) {
    // From https://motors.ctr-electronics.com/dyno/dynometer-testing/
    return new DCMotor(
        12, 5.01, 329, 2, Units.rotationsPerMinuteToRadiansPerSecond(7368), numMotors);
  }

  /**
   * Return a gearbox of Minion brushless motors.
   *
   * @param numMotors Number of motors in the gearbox.
   * @return A gearbox of Minion motors.
   */
  public static DCMotor getMinion(int numMotors) {
    // From https://motors.ctr-electronics.com/dyno/dynometer-testing/
    return new DCMotor(
        12, 3.17, 211, 2, Units.rotationsPerMinuteToRadiansPerSecond(7704), numMotors);
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
