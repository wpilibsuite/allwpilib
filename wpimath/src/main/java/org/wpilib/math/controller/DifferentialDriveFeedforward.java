// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.controller;

import org.wpilib.math.controller.proto.DifferentialDriveFeedforwardProto;
import org.wpilib.math.controller.struct.DifferentialDriveFeedforwardStruct;
import org.wpilib.math.linalg.VecBuilder;
import org.wpilib.math.numbers.N2;
import org.wpilib.math.system.LinearSystem;
import org.wpilib.math.system.Models;
import org.wpilib.util.protobuf.ProtobufSerializable;
import org.wpilib.util.struct.StructSerializable;

/** A helper class which computes the feedforward outputs for a differential drive drivetrain. */
public class DifferentialDriveFeedforward implements ProtobufSerializable, StructSerializable {
  private final LinearSystem<N2, N2, N2> m_plant;

  /** The linear velocity gain in volts per (meters per second). */
  public final double kvLinear;

  /** The linear acceleration gain in volts per (meters per second squared). */
  public final double kaLinear;

  /** The angular velocity gain in volts per (radians per second). */
  public final double kvAngular;

  /** The angular acceleration gain in volts per (radians per second squared). */
  public final double kaAngular;

  /**
   * Creates a new DifferentialDriveFeedforward with the specified parameters.
   *
   * @param kvLinear The linear velocity gain in volts per (meters per second).
   * @param kaLinear The linear acceleration gain in volts per (meters per second squared).
   * @param kvAngular The angular velocity gain in volts per (radians per second).
   * @param kaAngular The angular acceleration gain in volts per (radians per second squared).
   * @param trackwidth The distance between the differential drive's left and right wheels, in
   *     meters.
   */
  public DifferentialDriveFeedforward(
      double kvLinear, double kaLinear, double kvAngular, double kaAngular, double trackwidth) {
    // See Models.differentialDriveFromSysId(double, double, double, double, double)
    this(kvLinear, kaLinear, kvAngular * 2.0 / trackwidth, kaAngular * 2.0 / trackwidth);
  }

  /**
   * Creates a new DifferentialDriveFeedforward with the specified parameters.
   *
   * @param kvLinear The linear velocity gain in volts per (meters per second).
   * @param kaLinear The linear acceleration gain in volts per (meters per second squared).
   * @param kvAngular The angular velocity gain in volts per (meters per second).
   * @param kaAngular The angular acceleration gain in volts per (meters per second squared).
   */
  public DifferentialDriveFeedforward(
      double kvLinear, double kaLinear, double kvAngular, double kaAngular) {
    m_plant = Models.differentialDriveFromSysId(kvLinear, kaLinear, kvAngular, kaAngular);
    this.kvLinear = kvLinear;
    this.kaLinear = kaLinear;
    this.kvAngular = kvAngular;
    this.kaAngular = kaAngular;
  }

  /**
   * Calculates the differential drive feedforward inputs given velocity references.
   *
   * @param currentLeftVelocity The current left velocity of the differential drive in
   *     meters/second.
   * @param nextLeftVelocity The next left velocity of the differential drive in meters/second.
   * @param currentRightVelocity The current right velocity of the differential drive in
   *     meters/second.
   * @param nextRightVelocity The next right velocity of the differential drive in meters/second.
   * @param dt Discretization timestep in seconds.
   * @return A DifferentialDriveWheelVoltages object containing the computed feedforward voltages.
   */
  public DifferentialDriveWheelVoltages calculate(
      double currentLeftVelocity,
      double nextLeftVelocity,
      double currentRightVelocity,
      double nextRightVelocity,
      double dt) {
    var feedforward = new LinearPlantInversionFeedforward<>(m_plant, dt);
    var r = VecBuilder.fill(currentLeftVelocity, currentRightVelocity);
    var nextR = VecBuilder.fill(nextLeftVelocity, nextRightVelocity);
    var u = feedforward.calculate(r, nextR);
    return new DifferentialDriveWheelVoltages(u.get(0, 0), u.get(1, 0));
  }

  /** DifferentialDriveFeedforward struct for serialization. */
  public static final DifferentialDriveFeedforwardStruct struct =
      new DifferentialDriveFeedforwardStruct();

  /** DifferentialDriveFeedforward protobuf for serialization. */
  public static final DifferentialDriveFeedforwardProto proto =
      new DifferentialDriveFeedforwardProto();
}
