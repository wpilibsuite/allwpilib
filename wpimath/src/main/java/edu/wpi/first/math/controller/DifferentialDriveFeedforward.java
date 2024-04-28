// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.controller.proto.DifferentialDriveFeedforwardProto;
import edu.wpi.first.math.controller.struct.DifferentialDriveFeedforwardStruct;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.plant.LinearSystemId;
import edu.wpi.first.util.protobuf.ProtobufSerializable;
import edu.wpi.first.util.struct.StructSerializable;

/** A helper class which computes the feedforward outputs for a differential drive drivetrain. */
public class DifferentialDriveFeedforward implements ProtobufSerializable, StructSerializable {
  private final LinearSystem<N2, N2, N2> m_plant;

  /** The linear velocity gain in volts per (meters per second). */
  public final double m_kVLinear;

  /** The linear acceleration gain in volts per (meters per second squared). */
  public final double m_kALinear;

  /** The angular velocity gain in volts per (radians per second). */
  public final double m_kVAngular;

  /** The angular acceleration gain in volts per (radians per second squared). */
  public final double m_kAAngular;

  /**
   * Creates a new DifferentialDriveFeedforward with the specified parameters.
   *
   * @param kVLinear The linear velocity gain in volts per (meters per second).
   * @param kALinear The linear acceleration gain in volts per (meters per second squared).
   * @param kVAngular The angular velocity gain in volts per (radians per second).
   * @param kAAngular The angular acceleration gain in volts per (radians per second squared).
   * @param trackwidth The distance between the differential drive's left and right wheels, in
   *     meters.
   */
  public DifferentialDriveFeedforward(
      double kVLinear, double kALinear, double kVAngular, double kAAngular, double trackwidth) {
    // See LinearSystemId.identifyDrivetrainSystem(double, double, double, double, double)
    this(kVLinear, kALinear, kVAngular * 2.0 / trackwidth, kAAngular * 2.0 / trackwidth);
  }

  /**
   * Creates a new DifferentialDriveFeedforward with the specified parameters.
   *
   * @param kVLinear The linear velocity gain in volts per (meters per second).
   * @param kALinear The linear acceleration gain in volts per (meters per second squared).
   * @param kVAngular The angular velocity gain in volts per (meters per second).
   * @param kAAngular The angular acceleration gain in volts per (meters per second squared).
   */
  public DifferentialDriveFeedforward(
      double kVLinear, double kALinear, double kVAngular, double kAAngular) {
    m_plant = LinearSystemId.identifyDrivetrainSystem(kVLinear, kALinear, kVAngular, kAAngular);
    m_kVLinear = kVLinear;
    m_kALinear = kALinear;
    m_kVAngular = kVAngular;
    m_kAAngular = kAAngular;
  }

  /**
   * Calculates the differential drive feedforward inputs given velocity setpoints.
   *
   * @param currentLeftVelocity The current left velocity of the differential drive in
   *     meters/second.
   * @param nextLeftVelocity The next left velocity of the differential drive in meters/second.
   * @param currentRightVelocity The current right velocity of the differential drive in
   *     meters/second.
   * @param nextRightVelocity The next right velocity of the differential drive in meters/second.
   * @param dtSeconds Discretization timestep.
   * @return A DifferentialDriveWheelVoltages object containing the computed feedforward voltages.
   */
  public DifferentialDriveWheelVoltages calculate(
      double currentLeftVelocity,
      double nextLeftVelocity,
      double currentRightVelocity,
      double nextRightVelocity,
      double dtSeconds) {
    var feedforward = new LinearPlantInversionFeedforward<>(m_plant, dtSeconds);
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
