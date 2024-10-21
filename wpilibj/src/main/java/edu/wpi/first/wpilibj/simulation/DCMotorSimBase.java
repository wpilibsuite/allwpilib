// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static edu.wpi.first.units.Units.Amps;
import static edu.wpi.first.units.Units.KilogramSquareMeters;
import static edu.wpi.first.units.Units.NewtonMeters;
import static edu.wpi.first.units.Units.RPM;
import static edu.wpi.first.units.Units.Radians;
import static edu.wpi.first.units.Units.RadiansPerSecond;
import static edu.wpi.first.units.Units.RadiansPerSecondPerSecond;
import static edu.wpi.first.units.Units.Rotations;
import static edu.wpi.first.units.Units.Volts;

import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.plant.Gearbox;
import edu.wpi.first.units.measure.Angle;
import edu.wpi.first.units.measure.AngularAcceleration;
import edu.wpi.first.units.measure.AngularVelocity;
import edu.wpi.first.units.measure.Current;
import edu.wpi.first.units.measure.MomentOfInertia;
import edu.wpi.first.units.measure.MutAngle;
import edu.wpi.first.units.measure.MutAngularAcceleration;
import edu.wpi.first.units.measure.MutAngularVelocity;
import edu.wpi.first.units.measure.MutCurrent;
import edu.wpi.first.units.measure.MutTorque;
import edu.wpi.first.units.measure.MutVoltage;
import edu.wpi.first.units.measure.Torque;
import edu.wpi.first.units.measure.Voltage;

/** The base clase for a simulated DC motor mechanism. */
public abstract class DCMotorSimBase extends LinearSystemSim<N2, N1, N2> {
  /** The gearbox of the simulated DC motor mechanism. */
  protected final Gearbox m_gearbox;

  /** The moment of inertia of the simulated DC motor mechanism. */
  private final MomentOfInertia m_j;

  /** The angle of the simulated DC motor mechanism. */
  private final MutAngle m_angle = Radians.mutable(0.0);

  /** The angular velocity of the simulated DC motor mechanism. */
  protected final MutAngularVelocity m_angularVelocity = RadiansPerSecond.mutable(0.0);

  /** The angular acceleration of the simulated DC motor mechanism. */
  private final MutAngularAcceleration m_angularAcceleration =
      RadiansPerSecondPerSecond.mutable(0.0);

  /** The current draw of the simulated DC motor mechanism. */
  protected final MutCurrent m_currentDraw = Amps.mutable(0.0);

  /** The voltage of the simulated DC motor mechanism. */
  protected final MutVoltage m_voltage = Volts.mutable(0.0);

  /** The torque of the simulated DC motor mechanism. */
  protected final MutTorque m_torque = NewtonMeters.mutable(0.0);

  /**
   * Creates a simulated DC motor mechanism.
   *
   * @param plant The linear system that represents the simulated DC motor mechanism.
   * @param gearbox The gearbox of the simulated DC motor mechanism.
   * @param J The moment of inertia of the simulated DC motor mechanism.
   * @param measurementStdDevs The standard deviations of the measurements. Can be omitted if no
   *     noise is desired. If present must have 2 elements. The first element is for position. The
   *     second element is for velocity. The units should be radians for position and radians per
   *     second for velocity.
   */
  public DCMotorSimBase(
      LinearSystem<N2, N1, N2> plant,
      Gearbox gearbox,
      MomentOfInertia J,
      double... measurementStdDevs) {
    super(plant, measurementStdDevs);
    m_gearbox = gearbox;
    m_j = J;
  }

  /**
   * Sets the state of the simulated DC motor mechanism.
   *
   * @param angularPositionRad The new angular position in radians.
   * @param angularVelocityRadPerSec The new angular velocity in radians per second.
   */
  public void setState(double angularPositionRad, double angularVelocityRadPerSec) {
    setState(VecBuilder.fill(angularPositionRad, angularVelocityRadPerSec));
  }

  /**
   * Sets the state of the simulated DC motor mechanism.
   *
   * @param angularPosition The new angular position.
   * @param angularVelocity The new angular velocity.
   */
  public void setState(Angle angularPosition, AngularVelocity angularVelocity) {
    setState(angularPosition.in(Radians), angularVelocity.in(RadiansPerSecond));
  }

  /**
   * Sets the angular position of the simulated DC motor mechanism.
   *
   * @param angularPositionRad The new angular position in radians.
   */
  public void setAngle(double angularPositionRad) {
    setState(angularPositionRad, m_angularVelocity.in(RadiansPerSecond));
  }

  /**
   * Sets the angular position of the simulated DC motor mechanism.
   *
   * @param angularPosition The new angular position.
   */
  public void setAngle(Angle angularPosition) {
    setAngle(angularPosition.in(Radians));
  }

  /**
   * Sets the angular velocity of the simulated DC motor mechanism.
   *
   * @param angularVelocityRadPerSec The new angular velocity in radians per second.
   */
  public void setAngularVelocity(double angularVelocityRadPerSec) {
    setState(m_angle.in(Radians), angularVelocityRadPerSec);
  }

  /**
   * Sets the angular velocity of the simulated DC motor mechanism.
   *
   * @param angularVelocity The new angular velocity.
   */
  public void setAngularVelocity(AngularVelocity angularVelocity) {
    setAngularVelocity(angularVelocity.in(RadiansPerSecond));
  }

  /**
   * Returns the moment of inertia of the simulated DC motor mechanism.
   *
   * @return The moment of inertia of the simulated DC motor mechanism in kilogram-square meters.
   */
  public double getJKgMetersSquared() {
    return m_j.in(KilogramSquareMeters);
  }

  /**
   * Returns the moment of inertia of the simulated DC motor mechanism.
   *
   * @return The moment of inertia of the simulated DC motor mechanism.
   */
  public MomentOfInertia getJ() {
    return m_j;
  }

  /**
   * Returns the gearbox of the simulated DC motor mechanism.
   *
   * @return The gearbox of the simulated DC motor mechanism.
   */
  public Gearbox getGearbox() {
    return m_gearbox;
  }

  /**
   * Returns the angular position of the simulated DC motor mechanism.
   *
   * @return The angular position of the simulated DC motor mechanism in radians.
   */
  public double getAngularPositionRad() {
    return m_angle.in(Radians);
  }

  /**
   * Returns the angular position of the simulated DC motor mechanism.
   *
   * @return The angular position of the simulated DC motor mechanism in rotations.
   */
  public double getAngularPositionRotations() {
    return m_angle.in(Rotations);
  }

  /**
   * Returns the angular position of the simulated DC motor mechanism.
   *
   * @return The angular position of the simulated DC motor mechanism.
   */
  public Angle getAngularPosition() {
    return m_angle;
  }

  /**
   * Returns the angular velocity of the simulated DC motor mechanism.
   *
   * @return The angular velocity of the simulated DC motor mechanism in radians per second.
   */
  public double getAngularVelocityRadPerSec() {
    return m_angularVelocity.in(RadiansPerSecond);
  }

  /**
   * Returns the angular velocity of the simulated DC motor mechanism.
   *
   * @return The angular velocity of the simulated DC motor mechanism in RPM.
   */
  public double getAngularVelocityRPM() {
    return m_angularVelocity.in(RPM);
  }

  /**
   * Returns the angular velocity of the simulated DC motor mechanism.
   *
   * @return The angular velocity of the simulated DC motor mechanism.
   */
  public AngularVelocity getAngularVelocity() {
    return m_angularVelocity;
  }

  /**
   * Returns the angular acceleration of the simulated DC motor mechanism.
   *
   * @return The angular acceleration of the simulated DC motor mechanism in radians per second
   *     squared.
   */
  public double getAngularAccelerationRadPerSecSq() {
    return m_angularAcceleration.in(RadiansPerSecondPerSecond);
  }

  /**
   * Returns the angular acceleration of the simulated DC motor mechanism.
   *
   * @return The angular acceleration of the simulated DC motor mechanism.
   */
  public AngularAcceleration getAngularAcceleration() {
    return m_angularAcceleration;
  }

  /**
   * Returns the current draw of the simulated DC motor mechanism.
   *
   * @return The current draw of the simulated DC motor mechanism in amps.
   */
  public double getCurrentDrawAmps() {
    return m_currentDraw.in(Amps);
  }

  /**
   * Returns the current draw of the simulated DC motor mechanism.
   *
   * @return The current draw of the simulated DC motor mechanism.
   */
  public Current getCurrentDraw() {
    return m_currentDraw;
  }

  /**
   * Returns the torque of the simulated DC motor mechanism.
   *
   * @return The torque of the simulated DC motor mechanism in newton-meters.
   */
  public double getTorqueNewtonMeters() {
    return m_torque.in(NewtonMeters);
  }

  /**
   * Returns the torque of the simulated DC motor mechanism.
   *
   * @return The torque of the simulated DC motor mechanism.
   */
  public Torque getTorque() {
    return m_torque;
  }

  /**
   * Returns the voltage of the simulated DC motor mechanism.
   *
   * @return The voltage of the simulated DC motor mechanism in volts.
   */
  public double getVolts() {
    return m_voltage.in(Volts);
  }

  /**
   * Returns the voltage of the simulated DC motor mechanism.
   *
   * @return The voltage of the simulated DC motor mechanism.
   */
  public Voltage getVoltage() {
    return m_voltage;
  }

  @Override
  public void update(double dtSeconds) {
    super.update(dtSeconds);
    m_angle.mut_replace(getOutput(0), Radians);
    m_angularVelocity.mut_replace(getOutput(1), RadiansPerSecond);
    m_angularAcceleration.mut_replace(
        (m_plant.getA().times(m_x)).plus(m_plant.getB().times(m_u)).get(1, 0),
        RadiansPerSecondPerSecond);
  }
}
