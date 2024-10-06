// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static edu.wpi.first.units.Units.Amps;
import static edu.wpi.first.units.Units.Kilograms;
import static edu.wpi.first.units.Units.Meters;
import static edu.wpi.first.units.Units.MetersPerSecond;
import static edu.wpi.first.units.Units.MetersPerSecondPerSecond;
import static edu.wpi.first.units.Units.NewtonMeters;
import static edu.wpi.first.units.Units.Newtons;
import static edu.wpi.first.units.Units.Volts;

import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.plant.Wheel;
import edu.wpi.first.units.measure.Current;
import edu.wpi.first.units.measure.Distance;
import edu.wpi.first.units.measure.Force;
import edu.wpi.first.units.measure.LinearAcceleration;
import edu.wpi.first.units.measure.LinearVelocity;
import edu.wpi.first.units.measure.Mass;
import edu.wpi.first.units.measure.MutCurrent;
import edu.wpi.first.units.measure.MutDistance;
import edu.wpi.first.units.measure.MutForce;
import edu.wpi.first.units.measure.MutLinearAcceleration;
import edu.wpi.first.units.measure.MutLinearVelocity;
import edu.wpi.first.units.measure.MutTorque;
import edu.wpi.first.units.measure.MutVoltage;
import edu.wpi.first.units.measure.Voltage;

/** Represents a simulated wheel mechanism. */
public abstract class WheelSimBase extends LinearSystemSim<N2, N1, N2> {
  /** The wheel object containing the radius and the gearbox. */
  protected final Wheel m_wheel;

  /** The mass of the wheel. */
  private final Mass m_mass;

  /** The position of the wheel. */
  private final MutDistance m_position = Meters.mutable(0.0);

  /** The velocity of the wheel. */
  private final MutLinearVelocity m_velocity = MetersPerSecond.mutable(0.0);

  /** The acceleration of the wheel. */
  private final MutLinearAcceleration m_acceleration = MetersPerSecondPerSecond.mutable(0.0);

  /** The current draw of wheel. */
  protected final MutCurrent m_currentDraw = Amps.mutable(0.0);

  /** The voltage of the wheel. */
  protected final MutVoltage m_voltage = Volts.mutable(0.0);

  /** The force on the wheel. */
  protected final MutForce m_force = Newtons.mutable(0.0);

  /** The torque on the wheel. */
  protected final MutTorque m_torque = NewtonMeters.mutable(0.0);

  /**
   * Creates a simulated wheel mechanism.
   *
   * @param plant The linear system that represents the wheel
   * @param wheel The wheel object containing the radius and the gearbox.
   * @param mass The mass of the wheel.
   * @param measurementStdDevs The standard deviations of the measurements. Can be omitted if no
   *     noise is desired. If present must have 1 element for position.
   */
  @SuppressWarnings("this-escape")
  public WheelSimBase(
      LinearSystem<N2, N1, N2> plant, Wheel wheel, Mass mass, double... measurementStdDevs) {
    super(plant, measurementStdDevs);
    m_wheel = wheel;
    m_mass = mass;
  }

  /**
   * Sets the wheel's state.
   *
   * @param positionMeters The new position in meters.
   * @param velocityMetersPerSecond New velocity in meters per second.
   */
  public void setState(double positionMeters, double velocityMetersPerSecond) {
    setState(VecBuilder.fill(positionMeters, velocityMetersPerSecond));
  }

  /**
   * Sets the wheel's state.
   *
   * @param position The new position.
   * @param velocity The new velocity.
   */
  public void setState(Distance position, LinearVelocity velocity) {
    setState(position.in(Meters), velocity.in(MetersPerSecond));
  }

  /**
   * Sets the wheel's position.
   *
   * @param positionMeters The new position in meters.
   */
  public void setPosition(double positionMeters) {
    setState(positionMeters, m_velocity.in(MetersPerSecond));
  }

  /**
   * Sets the wheel's position.
   *
   * @param position The new position.
   */
  public void setPosition(Distance position) {
    setPosition(position.in(Meters));
  }

  /**
   * Sets the wheel's velocity.
   *
   * @param velocityMetersPerSecond The new velocity in meters per second.
   */
  public void setVelocity(double velocityMetersPerSecond) {
    setState(m_position.in(Meters), velocityMetersPerSecond);
  }

  /**
   * Sets the wheel's velocity.
   *
   * @param velocity The new velocity.
   */
  public void setVelocity(LinearVelocity velocity) {
    setVelocity(velocity.in(MetersPerSecond));
  }

  /**
   * Returns the mass of the wheel in kilograms.
   *
   * @return the mass of the wheel in kilograms.
   */
  public double getMassKilograms() {
    return m_mass.in(Kilograms);
  }

  /**
   * Returns the mass of the wheel.
   *
   * @return the mass of the wheel.
   */
  public Mass getMass() {
    return m_mass;
  }

  /**
   * Returns the wheel.
   *
   * @return The wheel.
   */
  public Wheel getWheel() {
    return m_wheel;
  }

  /**
   * Returns the position of the wheel in meters.
   *
   * @return The position of the wheel in meters.
   */
  public double getPositionMeters() {
    return m_position.in(Meters);
  }

  /**
   * Returns the position of the wheel.
   *
   * @return The position of the wheel.
   */
  public Distance getPosition() {
    return m_position;
  }

  /**
   * Returns the velocity of the wheel in meters per second.
   *
   * @return The velocity of the wheel in meters per second.
   */
  public double getVelocityMetersPerSecond() {
    return m_velocity.in(MetersPerSecond);
  }

  /**
   * Returns the velocity of the wheel.
   *
   * @return The velocity of the wheel.
   */
  public LinearVelocity getVelocity() {
    return m_velocity;
  }

  /**
   * Returns the acceleration of the wheel in meters per second squared.
   *
   * @return The acceleration of the wheel in meters per second squared.
   */
  public double getAccelerationMetersPerSecondSquared() {
    return m_acceleration.in(MetersPerSecondPerSecond);
  }

  /**
   * Returns the acceleration of the wheel.
   *
   * @return The acceleration of the wheel.
   */
  public LinearAcceleration getAcceleration() {
    return m_acceleration;
  }

  /**
   * Returns the wheel's current draw in Amps.
   *
   * @return The wheel's current draw in Amps.
   */
  public double getCurrentDrawAmps() {
    return m_currentDraw.in(Amps);
  }

  /**
   * Returns the wheel's current draw.
   *
   * @return The wheel's current draw.
   */
  public Current getCurrentDraw() {
    return m_currentDraw;
  }

  /**
   * Returns the force on the wheel in Newtons.
   *
   * @return The force on the wheel in Newtons.
   */
  public double getForceNewtons() {
    return m_force.in(Newtons);
  }

  /**
   * Returns the force on the wheel.
   *
   * @return The force on the wheel.
   */
  public Force getForce() {
    return m_force;
  }

  /**
   * Returns the voltage of the wheel in volts.
   *
   * @return The voltage of the wheel in volts.
   */
  public double getVoltageVolts() {
    return m_voltage.in(Volts);
  }

  /**
   * Returns the voltage of the wheel.
   *
   * @return The voltage of the wheel.
   */
  public Voltage getVoltage() {
    return m_voltage;
  }

  @Override
  public void update(double dtSeconds) {
    super.update(dtSeconds);
    m_position.mut_replace(getOutput(0), Meters);
    m_velocity.mut_replace(getOutput(1), MetersPerSecond);
    m_acceleration.mut_replace(
        (m_plant.getA().times(m_x)).plus(m_plant.getB().times(m_u)).get(0, 0),
        MetersPerSecondPerSecond);
    m_currentDraw.mut_replace(
        m_wheel.currentAmps(m_x.get(1, 0), m_u.get(0, 0)) * Math.signum(m_u.get(0, 0)), Amps);
  }
}
