// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static edu.wpi.first.units.Units.Amps;
import static edu.wpi.first.units.Units.KilogramSquareMeters;
import static edu.wpi.first.units.Units.NewtonMeters;
import static edu.wpi.first.units.Units.RPM;
import static edu.wpi.first.units.Units.RadiansPerSecond;
import static edu.wpi.first.units.Units.RadiansPerSecondPerSecond;
import static edu.wpi.first.units.Units.Volts;

import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.plant.Gearbox;
import edu.wpi.first.units.measure.AngularAcceleration;
import edu.wpi.first.units.measure.AngularVelocity;
import edu.wpi.first.units.measure.Current;
import edu.wpi.first.units.measure.MomentOfInertia;
import edu.wpi.first.units.measure.MutAngularAcceleration;
import edu.wpi.first.units.measure.MutAngularVelocity;
import edu.wpi.first.units.measure.MutCurrent;
import edu.wpi.first.units.measure.MutTorque;
import edu.wpi.first.units.measure.MutVoltage;
import edu.wpi.first.units.measure.Torque;
import edu.wpi.first.units.measure.Voltage;

/** The base class for a simulated flywheel mechanism. */
public abstract class FlywheelSimBase extends LinearSystemSim<N1, N1, N1> {
  /** The gearbox of the simulated flywheel mechanism. */
  protected final Gearbox m_gearbox;

  /** The moment of inertia of simulated the flywheel mechanism. */
  private final MomentOfInertia m_j;

  /** The angular velocity of the simulated flywheel mechanism. */
  protected final MutAngularVelocity m_angularVelocity = RadiansPerSecond.mutable(0.0);

  /** The angular acceleration of the simulated flywheel mechanism. */
  private final MutAngularAcceleration m_angularAcceleration =
      RadiansPerSecondPerSecond.mutable(0.0);

  /** The current draw of the simulated flywheel mechanism. */
  protected final MutCurrent m_currentDraw = Amps.mutable(0.0);

  /** The voltage of the simulated flywheel mechanism. */
  protected final MutVoltage m_voltage = Volts.mutable(0.0);

  /** The torque of the simulated flywheel mechanism. */
  protected final MutTorque m_torque = NewtonMeters.mutable(0.0);

  /**
   * Creates a simulated flywheel mechanism.
   *
   * @param plant The linear system that represents the simulated flywheel mechanism.
   * @param gearbox The gearbox of the simulated flywheel mechanism.
   * @param J The moment of inertia of the simulated flywheel mechanism.
   * @param measurementStdDevs The standard deviations of the measurements. Can be omitted if no
   *     noise is desired. If present must have 1 element for velocity. The units should be radians
   *     per second.
   */
  public FlywheelSimBase(
      LinearSystem<N1, N1, N1> plant,
      Gearbox gearbox,
      MomentOfInertia J,
      double... measurementStdDevs) {
    super(plant, measurementStdDevs);
    m_gearbox = gearbox;
    m_j = J;
  }

  /**
   * Sets the angular velocity of the simulated flywheel mechanism.
   *
   * @param velocityRadPerSec The new angular velocity in radians per second.
   */
  public void setAngularVelocity(double velocityRadPerSec) {
    setState(VecBuilder.fill(velocityRadPerSec));
  }

  /**
   * Sets the angular velocity of the simulated flywheel mechanism.
   *
   * @param velocity The new angular velocity.
   */
  public void setAngularVelocity(AngularVelocity velocity) {
    setAngularVelocity(velocity.in(RadiansPerSecond));
  }

  /**
   * Returns the moment of inertia of the simulated flywheel mechanism.
   *
   * @return The moment of inertia of the simulated flywheel mechanism in kilogram-square meters.
   */
  public double getJKgMetersSquared() {
    return m_j.in(KilogramSquareMeters);
  }

  /**
   * Returns the moment of inertia of the simulatd flywheel mechanism.
   *
   * @return The moment of inertia of the simulated flywheel mechanism.
   */
  public MomentOfInertia getJ() {
    return m_j;
  }

  /**
   * Returns the gearbox of the simulated flywheel mechanism.
   *
   * @return The gearbox of the simulated flywheel mechanism.
   */
  public Gearbox getGearbox() {
    return m_gearbox;
  }

  /**
   * Returns the angular velocity of the simulated flywheel mechanism.
   *
   * @return The angular velocity of the simulated flywheel mechanism in radians per second.
   */
  public double getAngularVelocityRadPerSec() {
    return m_angularVelocity.in(RadiansPerSecond);
  }

  /**
   * Returns the angular velocity of the simulated flywheel mechanism.
   *
   * @return The angular velocity of the simulated flywheel mechanism in RPM.
   */
  public double getAngularVelocityRPM() {
    return m_angularVelocity.in(RPM);
  }

  /**
   * Returns the angular velocity of the simulated flywheel mechanism.
   *
   * @return The angular velocity of the simulated flywheel mechanism.
   */
  public AngularVelocity getAngularVelocity() {
    return m_angularVelocity;
  }

  /**
   * Returns the angular acceleration of the simulated flywheel mechanism.
   *
   * @return The angular acceleration of the simulated flywheel mechanism in radians per squared.
   */
  public double getAngularAccelerationRadPerSecSq() {
    return m_angularAcceleration.in(RadiansPerSecondPerSecond);
  }

  /**
   * Returns the angular acceleration of the simulated flywheel mechanism.
   *
   * @return The angular acceleration of the simulated flywheel mechanism.
   */
  public AngularAcceleration getAngularAcceleration() {
    return m_angularAcceleration;
  }

  /**
   * Returns the current draw of the simulated flywheel mechanism.
   *
   * @return The current draw of the simulated flywheel mechanism in amps.
   */
  public double getCurrentDrawAmps() {
    return m_currentDraw.in(Amps);
  }

  /**
   * Returns the current draw of the simulated flywheel mechanism.
   *
   * @return The current draw of the simulated flywheel mechanism.
   */
  public Current getCurrentDraw() {
    return m_currentDraw;
  }

  /**
   * Returns the torque of the simulated flywheel mechanism.
   *
   * @return The torque of the simulated flywheel mechanism in newton-meters.
   */
  public double getTorqueNewtonMeters() {
    return m_torque.in(NewtonMeters);
  }

  /**
   * Returns the torque of the simulated flywheel mechanism.
   *
   * @return The torque of the simulated flywheel mechanism.
   */
  public Torque getTorque() {
    return m_torque;
  }

  /**
   * Returns the voltage of the simulated flywheel mechanism.
   *
   * @return The voltage of the simulated flywheel mechanism in volts.
   */
  public double getVoltageVolts() {
    return m_voltage.in(Volts);
  }

  /**
   * Returns the voltage of the simulated flywheel mechanism.
   *
   * @return The voltage of the simulated flywheel mechanism.
   */
  public Voltage getVoltage() {
    return m_voltage;
  }

  @Override
  public void update(double dtSeconds) {
    super.update(dtSeconds);
    m_angularVelocity.mut_replace(getOutput(0), RadiansPerSecond);
    m_angularAcceleration.mut_replace(
        (m_plant.getA().times(m_x)).plus(m_plant.getB().times(m_u)).get(0, 0),
        RadiansPerSecondPerSecond);
  }
}
