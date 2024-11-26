// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static edu.wpi.first.units.Units.Radians;
import static edu.wpi.first.units.Units.RadiansPerSecond;
import static edu.wpi.first.units.Units.RadiansPerSecondPerSecond;

import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.plant.Gearbox;
import edu.wpi.first.math.util.Units;
import edu.wpi.first.units.measure.Angle;
import edu.wpi.first.units.measure.AngularAcceleration;
import edu.wpi.first.units.measure.AngularVelocity;
import edu.wpi.first.wpilibj.RobotController;

/** Represents a simulated Gearbox mechanism. */
public class GearboxSim extends LinearSystemSim<N2, N1, N2> {
  // Gearbox for the system.
  private final Gearbox m_gearbox;

  // The moment of inertia for the Gearbox mechanism.
  private final double m_jKgMetersSquared;

  /**
   * Creates a simulated Gearbox mechanism.
   *
   * @param plant The linear system representing the Gearbox. This system can be created with {@link
   *     edu.wpi.first.math.system.plant.LinearSystemId#createGearboxSystem(Gearbox, double)} or
   *     {@link edu.wpi.first.math.system.plant.LinearSystemId#identifyPositionSystem(double,
   *     double)}. If {@link
   *     edu.wpi.first.math.system.plant.LinearSystemId#identifyPositionSystem(double, double)} is
   *     used, the distance unit must be radians.
   * @param gearbox The gearbox of the system.
   * @param measurementStdDevs The standard deviations of the measurements. Can be omitted if no
   *     noise is desired. If present must have 2 elements. The first element is for position. The
   *     second element is for velocity.
   */
  public GearboxSim(LinearSystem<N2, N1, N2> plant, Gearbox gearbox, double... measurementStdDevs) {
    super(plant, measurementStdDevs);
    m_gearbox = gearbox;

    // By theorem 6.10.1 of
    // https://file.tavsys.net/control/controls-engineering-in-frc.pdf,
    // the DC motor state-space model is:
    //
    // dx/dt = -nG²Kₜ/(KᵥRJ)x + nGKₜ/(RJ)u
    // A = -nG²Kₜ/(KᵥRJ)
    // B = nGKₜ/(RJ)
    //
    // Solve for J.
    //
    // B = nGKₜ/(RJ)
    // J = nGKₜ/(RB)
    m_jKgMetersSquared =
        m_gearbox.numMotors
            * m_gearbox.getGearboxReduction()
            * gearbox.dcMotor.kt.baseUnitMagnitude()
            / (gearbox.dcMotor.internalResistance.baseUnitMagnitude() * plant.getB(1, 0));
  }

  /**
   * Sets the state of the Gearbox.
   *
   * @param angularPositionRad The new position in radians.
   * @param angularVelocityRadPerSec The new velocity in radians per second.
   */
  public void setState(double angularPositionRad, double angularVelocityRadPerSec) {
    setState(VecBuilder.fill(angularPositionRad, angularVelocityRadPerSec));
  }

  /**
   * Sets the Gearbox's angular position.
   *
   * @param angularPositionRad The new position in radians.
   */
  public void setAngle(double angularPositionRad) {
    setState(angularPositionRad, getAngularVelocityRadPerSec());
  }

  /**
   * Sets the Gearbox's angular velocity.
   *
   * @param angularVelocityRadPerSec The new velocity in radians per second.
   */
  public void setAngularVelocity(double angularVelocityRadPerSec) {
    setState(getAngularPositionRad(), angularVelocityRadPerSec);
  }

  /**
   * Returns the moment of inertia of the Gearbox.
   *
   * @return The Gearbox's moment of inertia.
   */
  public double getJKgMetersSquared() {
    return m_jKgMetersSquared;
  }

  /**
   * Returns the gearbox for the system.
   *
   * @return The gearbox for the system.
   */
  public Gearbox getGearbox() {
    return m_gearbox;
  }

  /**
   * Returns the Gearbox's position.
   *
   * @return The Gearbox's position.
   */
  public double getAngularPositionRad() {
    return getOutput(0);
  }

  /**
   * Returns the Gearbox's position in rotations.
   *
   * @return The Gearbox's position in rotations.
   */
  public double getAngularPositionRotations() {
    return Units.radiansToRotations(getAngularPositionRad());
  }

  /**
   * Returns the Gearbox's position.
   *
   * @return The Gearbox's position
   */
  public Angle getAngularPosition() {
    return Radians.of(getAngularPositionRad());
  }

  /**
   * Returns the Gearbox's velocity.
   *
   * @return The Gearbox's velocity.
   */
  public double getAngularVelocityRadPerSec() {
    return getOutput(1);
  }

  /**
   * Returns the Gearbox's velocity in RPM.
   *
   * @return The Gearbox's velocity in RPM.
   */
  public double getAngularVelocityRPM() {
    return Units.radiansPerSecondToRotationsPerMinute(getAngularVelocityRadPerSec());
  }

  /**
   * Returns the Gearbox's velocity.
   *
   * @return The Gearbox's velocity
   */
  public AngularVelocity getAngularVelocity() {
    return RadiansPerSecond.of(getAngularVelocityRadPerSec());
  }

  /**
   * Returns the Gearbox's acceleration in Radians Per Second Squared.
   *
   * @return The Gearbox's acceleration in Radians Per Second Squared.
   */
  public double getAngularAccelerationRadPerSecSq() {
    var acceleration = (m_plant.getA().times(m_x)).plus(m_plant.getB().times(m_u));
    return acceleration.get(0, 0);
  }

  /**
   * Returns the Gearbox's acceleration.
   *
   * @return The Gearbox's acceleration.
   */
  public AngularAcceleration getAngularAcceleration() {
    return RadiansPerSecondPerSecond.of(getAngularAccelerationRadPerSecSq());
  }

  /**
   * Returns the Gearbox's torque in Newton-Meters.
   *
   * @return The Gearbox's torque in Newton-Meters.
   */
  public double getTorqueNewtonMeters() {
    return getAngularAccelerationRadPerSecSq() * m_jKgMetersSquared;
  }

  /**
   * Returns the Gearbox's net current draw.
   *
   * @return The Gearbox's net current draw.
   */
  public double getCurrentDrawAmps() {
    return m_gearbox.getCurrentAmps(getTorqueNewtonMeters()) * Math.signum(m_u.get(0, 0));
  }

  /**
   * Gets the input voltage for each of the Gearbox's motors.
   *
   * @return The input voltage for each of the Gearbox's motors.
   */
  public double getInputVoltage() {
    return getInput(0);
  }

  /**
   * Sets the input voltage for each of the Gearbox's motors.
   *
   * @param volts The input voltage for each of the Gearbox's motors.
   */
  public void setInputVoltage(double volts) {
    setInput(volts);
    clampInput(RobotController.getBatteryVoltage());
  }
}
