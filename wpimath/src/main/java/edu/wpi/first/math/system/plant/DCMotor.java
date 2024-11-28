// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.system.plant;

import static edu.wpi.first.units.Units.Amps;
import static edu.wpi.first.units.Units.NewtonMeters;
import static edu.wpi.first.units.Units.RadiansPerSecond;
import static edu.wpi.first.units.Units.Volts;

import edu.wpi.first.math.system.plant.proto.DCMotorProto;
import edu.wpi.first.math.system.plant.struct.DCMotorStruct;
import edu.wpi.first.units.AngularVelocityUnit;
import edu.wpi.first.units.CurrentUnit;
import edu.wpi.first.units.TorqueUnit;
import edu.wpi.first.units.VoltageUnit;
import edu.wpi.first.units.measure.AngularVelocity;
import edu.wpi.first.units.measure.Current;
import edu.wpi.first.units.measure.Per;
import edu.wpi.first.units.measure.Resistance;
import edu.wpi.first.units.measure.Torque;
import edu.wpi.first.units.measure.Voltage;
import edu.wpi.first.util.protobuf.ProtobufSerializable;
import edu.wpi.first.util.struct.StructSerializable;

/** Represents the properties and models the behavior of a DC motor. */
public class DCMotor implements ProtobufSerializable, StructSerializable {
  /** Voltage at which the motor constants were measured. */
  public final Voltage nominalVoltage;

  /** Torque when stalled. */
  public final Torque stallTorque;

  /** Current draw when stalled. */
  public final Current stallCurrent;

  /** Current draw under no load. */
  public final Current freeCurrent;

  /** Angular velocity under no load. */
  public final AngularVelocity freeSpeed;

  /** Motor internal resistance. */
  public final Resistance internalResistance;

  /** Motor velocity constant. */
  public final Per<AngularVelocityUnit, VoltageUnit> kv;

  /** Motor torque constant. */
  public final Per<TorqueUnit, CurrentUnit> kt;

  /** DCMotor protobuf for serialization. */
  public static final DCMotorProto proto = new DCMotorProto();

  /** DCMotor struct for serialization. */
  public static final DCMotorStruct struct = new DCMotorStruct();

  /**
   * Constructs a DC motor.
   *
   * @param nominalVoltageVolts Voltage at which the motor constants were measured in Volts.
   * @param stallTorqueNewtonMeters Torque when stalled in Newton-Meters.
   * @param stallCurrentAmps Current draw when stalled in Amps.
   * @param freeCurrentAmps Current draw under no load in Amps.
   * @param freeSpeedRadPerSec Angular velocity under no load in Radians per Second.
   */
  public DCMotor(
      double nominalVoltageVolts,
      double stallTorqueNewtonMeters,
      double stallCurrentAmps,
      double freeCurrentAmps,
      double freeSpeedRadPerSec) {
    this(
        Volts.of(nominalVoltageVolts),
        NewtonMeters.of(stallTorqueNewtonMeters),
        Amps.of(stallCurrentAmps),
        Amps.of(freeCurrentAmps),
        RadiansPerSecond.of(freeSpeedRadPerSec));
  }

  /**
   * Constructs a DC motor.
   *
   * @param nominalVoltage Voltage at which the motor constants were measured.
   * @param stallTorque Torque when stalled.
   * @param stallCurrent Current draw when stalled.
   * @param freeCurrent Current draw under no load.
   * @param freeSpeed Angular velocity under no load.
   */
  public DCMotor(
      Voltage nominalVoltage,
      Torque stallTorque,
      Current stallCurrent,
      Current freeCurrent,
      AngularVelocity freeSpeed) {
    this.nominalVoltage = nominalVoltage;
    this.stallTorque = stallTorque;
    this.stallCurrent = stallCurrent;
    this.freeCurrent = freeCurrent;
    this.freeSpeed = freeSpeed;
    internalResistance = nominalVoltage.div(stallCurrent);
    kv = freeSpeed.div(nominalVoltage.minus(internalResistance.times(freeCurrent)));
    kt = stallTorque.div(stallCurrent);
  }

  /**
   * Calculate the input voltage of the motor for a given torque and angular velocity.
   *
   * @param torqueNewtonMeters The torque produced by the motor.
   * @param angularVelocityRadiansPerSec The current angular velocity of the motor.
   * @return The voltage of the motor.
   */
  public double getVoltageInputVolts(
      double torqueNewtonMeters, double angularVelocityRadiansPerSec) {
    return 1.0 / kv.baseUnitMagnitude() * angularVelocityRadiansPerSec
        + 1.0
            / kt.baseUnitMagnitude()
            * internalResistance.baseUnitMagnitude()
            * torqueNewtonMeters;
  }

  /**
   * Calculates the angular velocity in Radians per Second produced by the motor at a given torque
   * and input voltage.
   *
   * @param torqueNewtonMeters The torque produced by the motor.
   * @param voltageInputVolts The voltage applied to the motor.
   * @return The angular velocity of the motor.
   */
  public double getAngularVelocityRadiansPerSecond(
      double torqueNewtonMeters, double voltageInputVolts) {
    return voltageInputVolts * kv.baseUnitMagnitude()
        - 1.0
            / kt.baseUnitMagnitude()
            * torqueNewtonMeters
            * internalResistance.baseUnitMagnitude()
            * kv.baseUnitMagnitude();
  }

  /**
   * Calculate torque in Newton-Meters produced by the motor at a given angular velocity and input
   * voltage.
   *
   * @param angularVelocityRadiansPerSec The current angular velocity of the motor.
   * @param voltageInputVolts The voltage applied to the motor.
   * @return The torque output.
   */
  public double getTorqueNewtonMeters(
      double angularVelocityRadiansPerSec, double voltageInputVolts) {
    return kt.baseUnitMagnitude()
        * (voltageInputVolts - (angularVelocityRadiansPerSec / kv.baseUnitMagnitude()));
  }

  /**
   * Calculate current drawn in Amps by motor for a given torque.
   *
   * @param torqueNewtonMeters The torque produced by the motor.
   * @return The current drawn by the motor.
   */
  public double getCurrentAmps(double torqueNewtonMeters) {
    return torqueNewtonMeters / kt.baseUnitMagnitude();
  }
}
