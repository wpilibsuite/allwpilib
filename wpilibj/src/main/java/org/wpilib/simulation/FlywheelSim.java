// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.simulation;

import org.wpilib.math.linalg.VecBuilder;
import org.wpilib.math.numbers.N1;
import org.wpilib.math.system.DCMotor;
import org.wpilib.math.system.LinearSystem;
import org.wpilib.math.system.Models;
import org.wpilib.system.RobotController;

/** Represents a simulated flywheel mechanism. */
public class FlywheelSim extends LinearSystemSim<N1, N1, N1> {
  // Gearbox for the flywheel.
  private final DCMotor m_gearbox;

  // The gearing from the motors to the output.
  private final double m_gearing;

  // The moment of inertia for the flywheel mechanism.
  private final double m_j;

  /**
   * Creates a simulated flywheel mechanism.
   *
   * @param plant The linear system that represents the flywheel. Use either {@link
   *     Models#flywheelFromPhysicalConstants(DCMotor, double, double)} if using physical constants
   *     or {@link Models#flywheelFromSysId(double, double)} if using system characterization.
   * @param gearbox The type of and number of motors in the flywheel gearbox.
   * @param measurementStdDevs The standard deviations of the measurements. Can be omitted if no
   *     noise is desired. If present must have 1 element for velocity.
   */
  public FlywheelSim(
      LinearSystem<N1, N1, N1> plant, DCMotor gearbox, double... measurementStdDevs) {
    super(plant, measurementStdDevs);
    m_gearbox = gearbox;

    // By theorem 6.10.1 of https://file.tavsys.net/control/controls-engineering-in-frc.pdf,
    // the flywheel state-space model is:
    //
    //   dx/dt = -G²Kₜ/(KᵥRJ)x + (GKₜ)/(RJ)u
    //   A = -G²Kₜ/(KᵥRJ)
    //   B = GKₜ/(RJ)
    //
    // Solve for G.
    //
    //   A/B = -G/Kᵥ
    //   G = -KᵥA/B
    //
    // Solve for J.
    //
    //   B = GKₜ/(RJ)
    //   J = GKₜ/(RB)
    m_gearing = -gearbox.Kv * plant.getA(0, 0) / plant.getB(0, 0);
    m_j = m_gearing * gearbox.Kt / (gearbox.R * plant.getB(0, 0));
  }

  /**
   * Sets the flywheel's angular velocity.
   *
   * @param velocity The new velocity in radians per second.
   */
  public void setAngularVelocity(double velocity) {
    setState(VecBuilder.fill(velocity));
  }

  /**
   * Returns the gear ratio of the flywheel.
   *
   * @return the flywheel's gear ratio.
   */
  public double getGearing() {
    return m_gearing;
  }

  /**
   * Returns the moment of inertia.
   *
   * @return The flywheel's moment of inertia in kg-m².
   */
  public double getJ() {
    return m_j;
  }

  /**
   * Returns the gearbox for the flywheel.
   *
   * @return The flywheel's gearbox.
   */
  public DCMotor getGearbox() {
    return m_gearbox;
  }

  /**
   * Returns the flywheel's velocity.
   *
   * @return The flywheel's velocity in rad/s.
   */
  public double getAngularVelocity() {
    return getOutput(0);
  }

  /**
   * Returns the flywheel's acceleration.
   *
   * @return The flywheel's acceleration in rad/s².
   */
  public double getAngularAcceleration() {
    var acceleration = (m_plant.getA().times(m_x)).plus(m_plant.getB().times(m_u));
    return acceleration.get(0, 0);
  }

  /**
   * Returns the flywheel's torque.
   *
   * @return The flywheel's torque in Newton-meters.
   */
  public double getTorque() {
    return getAngularAcceleration() * m_j;
  }

  /**
   * Returns the flywheel's current draw.
   *
   * @return The flywheel's current draw in amps.
   */
  public double getCurrentDraw() {
    // I = V / R - omega / (Kv * R)
    // Reductions are output over input, so a reduction of 2:1 means the motor is spinning
    // 2x faster than the flywheel
    return m_gearbox.getCurrent(m_x.get(0, 0) * m_gearing, m_u.get(0, 0))
        * Math.signum(m_u.get(0, 0));
  }

  /**
   * Gets the input voltage for the flywheel.
   *
   * @return The flywheel's input voltage.
   */
  public double getInputVoltage() {
    return getInput(0);
  }

  /**
   * Sets the input voltage for the flywheel.
   *
   * @param volts The input voltage.
   */
  public void setInputVoltage(double volts) {
    setInput(volts);
    clampInput(RobotController.getBatteryVoltage());
  }
}
