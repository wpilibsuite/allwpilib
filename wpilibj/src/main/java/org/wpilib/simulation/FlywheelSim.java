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
   * @throws IllegalArgumentException if the plant's A(0, 0) or B(0, 0) entry is zero, which leaves
   *     the gearing and moment of inertia undetermined. A(0, 0) is zero for a plant built from
   *     SysId constants with kV = 0.
   */
  public FlywheelSim(
      LinearSystem<N1, N1, N1> plant, DCMotor gearbox, double... measurementStdDevs) {
    super(plant, measurementStdDevs);

    if (plant.getA(0, 0) == 0.0) {
      throw new IllegalArgumentException(
          "plant must have nonzero velocity damping A(0, 0); a plant built with kV = 0 doesn't "
              + "determine the gearing.");
    }
    if (plant.getB(0, 0) == 0.0) {
      throw new IllegalArgumentException("plant must have nonzero input gain B(0, 0).");
    }

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
   * @return The flywheel's gear ratio.
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
   * <p>This is the current drawn from the battery, which differs from the current through the motor
   * by the duty cycle the motor controller is applying. A negative value means the flywheel is
   * regenerating and returning current to the battery.
   *
   * @return The flywheel's current draw in amps.
   */
  public double getCurrentDraw() {
    // Reductions are greater than 1, so a reduction of 10:1 would mean the motor is
    // spinning 10x faster than the output.
    //
    // The current through the motor is I = V/R - ω/(KᵥR), where V is the voltage across
    // the motor terminals.
    //
    // The motor controller produces V by PWMing the battery voltage at duty cycle
    // D = V/V_batt. An ideal H-bridge conserves power, so V_batt·I_supply = V·I, giving
    //
    //   I_supply = D·I
    //
    // Scaling by D also makes the result continuous through V = 0, where the motor is
    // braking and its circulating current isn't drawn from the battery.
    double motorVelocity = m_x.get(0, 0) * m_gearing;
    var appliedVoltage = m_u.get(0, 0);
    var dutyCycle = appliedVoltage / RobotController.getBatteryVoltage();
    return m_gearbox.getCurrent(motorVelocity, appliedVoltage) * dutyCycle;
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
