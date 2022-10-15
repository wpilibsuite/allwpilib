// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import edu.wpi.first.math.MatBuilder;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.system.LinearSystem;

/**
 * Filters the provided voltages to limit a differential drive's linear and angular acceleration.
 *
 * <p>The differential drive model can be created via the functions in {@link
 * edu.wpi.first.math.system.plant.LinearSystemId}.
 */
public class DifferentialDriveAccelerationLimiter {
  private final LinearSystem<N2, N2, N2> m_system;
  private final double m_trackwidth;
  private final double m_minLinearAccel;
  private final double m_maxLinearAccel;
  private final double m_maxAngularAccel;

  /**
   * Constructs a DifferentialDriveAccelerationLimiter.
   *
   * @param system The differential drive dynamics.
   * @param trackwidth The distance between the differential drive's left and right wheels in
   *     meters.
   * @param maxLinearAccel The maximum linear acceleration in meters per second squared.
   * @param maxAngularAccel The maximum angular acceleration in radians per second squared.
   */
  public DifferentialDriveAccelerationLimiter(
      LinearSystem<N2, N2, N2> system,
      double trackwidth,
      double maxLinearAccel,
      double maxAngularAccel) {
    this(system, trackwidth, -maxLinearAccel, maxLinearAccel, maxAngularAccel);
  }

  /**
   * Constructs a DifferentialDriveAccelerationLimiter.
   *
   * @param system The differential drive dynamics.
   * @param trackwidth The distance between the differential drive's left and right wheels in
   *     meters.
   * @param minLinearAccel The minimum (most negative) linear acceleration in meters per second
   *     squared.
   * @param maxLinearAccel The maximum (most positive) linear acceleration in meters per second
   *     squared.
   * @param maxAngularAccel The maximum angular acceleration in radians per second squared.
   * @throws IllegalArgumentException if minimum linear acceleration is greater than maximum linear
   *     acceleration
   */
  public DifferentialDriveAccelerationLimiter(
      LinearSystem<N2, N2, N2> system,
      double trackwidth,
      double minLinearAccel,
      double maxLinearAccel,
      double maxAngularAccel) {
    if (minLinearAccel > maxLinearAccel) {
      throw new IllegalArgumentException("maxLinearAccel must be greater than minLinearAccel");
    }
    m_system = system;
    m_trackwidth = trackwidth;
    m_minLinearAccel = minLinearAccel;
    m_maxLinearAccel = maxLinearAccel;
    m_maxAngularAccel = maxAngularAccel;
  }

  /**
   * Returns the next voltage pair subject to acceleration constraints.
   *
   * @param leftVelocity The left wheel velocity in meters per second.
   * @param rightVelocity The right wheel velocity in meters per second.
   * @param leftVoltage The unconstrained left motor voltage.
   * @param rightVoltage The unconstrained right motor voltage.
   * @return The constrained wheel voltages.
   */
  public DifferentialDriveWheelVoltages calculate(
      double leftVelocity, double rightVelocity, double leftVoltage, double rightVoltage) {
    var u = new MatBuilder<>(Nat.N2(), Nat.N1()).fill(leftVoltage, rightVoltage);

    // Find unconstrained wheel accelerations
    var x = new MatBuilder<>(Nat.N2(), Nat.N1()).fill(leftVelocity, rightVelocity);
    var dxdt = m_system.getA().times(x).plus(m_system.getB().times(u));

    // Convert from wheel accelerations to linear and angular accelerations
    //
    // a = (dxdt(0) + dx/dt(1)) / 2
    //   = 0.5 dxdt(0) + 0.5 dxdt(1)
    //
    // α = (dxdt(1) - dxdt(0)) / trackwidth
    //   = -1/trackwidth dxdt(0) + 1/trackwidth dxdt(1)
    //
    // [a] = [          0.5           0.5][dxdt(0)]
    // [α]   [-1/trackwidth  1/trackwidth][dxdt(1)]
    //
    // accels = M dxdt where M = [0.5, 0.5; -1/trackwidth, 1/trackwidth]
    var M =
        new MatBuilder<>(Nat.N2(), Nat.N2())
            .fill(0.5, 0.5, -1.0 / m_trackwidth, 1.0 / m_trackwidth);
    var accels = M.times(dxdt);

    // Constrain the linear and angular accelerations
    if (accels.get(0, 0) > m_maxLinearAccel) {
      accels.set(0, 0, m_maxLinearAccel);
    } else if (accels.get(0, 0) < m_minLinearAccel) {
      accels.set(0, 0, m_minLinearAccel);
    }
    if (accels.get(1, 0) > m_maxAngularAccel) {
      accels.set(1, 0, m_maxAngularAccel);
    } else if (accels.get(1, 0) < -m_maxAngularAccel) {
      accels.set(1, 0, -m_maxAngularAccel);
    }

    // Convert the constrained linear and angular accelerations back to wheel
    // accelerations
    dxdt = M.solve(accels);

    // Find voltages for the given wheel accelerations
    //
    // dx/dt = Ax + Bu
    // u = B⁻¹(dx/dt - Ax)
    u = m_system.getB().solve(dxdt.minus(m_system.getA().times(x)));

    return new DifferentialDriveWheelVoltages(u.get(0, 0), u.get(1, 0));
  }
}
