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
import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.math.system.plant.LinearSystemId;
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

/** Represents a simulated flywheel mechanism. */
public abstract class FlywheelSimBase extends LinearSystemSim<N1, N1, N1> {
  /**  Gearbox for the flywheel. */
  protected final DCMotor m_gearbox;

  /** The gearing from the motors to the output. */
  protected final double m_gearing;

  /** The moment of inertia for the flywheel mechanism. */
  private final MomentOfInertia m_j;

  /** The angular velocity of the flywheel mechanism. */
  protected final MutAngularVelocity m_angularVelocity = RadiansPerSecond.mutable(0.0);

  /** The angular acceleration of the flywheel mechanism. */
  private final MutAngularAcceleration m_angularAcceleration =
      RadiansPerSecondPerSecond.mutable(0.0);

  /** The current draw of flywheel. */
  protected final MutCurrent m_currentDraw = Amps.mutable(0.0);

  /** The voltage of the flywheel. */
  protected final MutVoltage m_voltage = Volts.mutable(0.0);

  /** The torque on the flywheel. */
  protected final MutTorque m_torque = NewtonMeters.mutable(0.0);

  /**
   * Creates a simulated flywheel mechanism.
   *
   * @param plant The linear system that represents the flywheel. Use either {@link
   *     LinearSystemId#createFlywheelSystem(DCMotor, double, double)} if using physical constants
   *     or {@link LinearSystemId#identifyVelocitySystem(double, double)} if using system
   *     characterization.
   * @param gearbox The type of and number of motors in the flywheel gearbox.
   * @param gearing The gearing from the motors to the output.
   * @param J The moment of inertia for the flywheel mechanism.
   * @param measurementStdDevs The standard deviations of the measurements. Can be omitted if no
   *     noise is desired. If present must have 1 element for velocity.
   */
  public FlywheelSimBase(
      LinearSystem<N1, N1, N1> plant,
      DCMotor gearbox,
      double gearing,
      MomentOfInertia J,
      double... measurementStdDevs) {
    super(plant, measurementStdDevs);
    m_gearbox = gearbox;
    m_gearing = gearing;
    m_j = J;
  }

  /**
   * Sets the flywheel's angular velocity.
   *
   * @param velocityRadPerSec The new angular velocity in radians per second.
   */
  public void setAngularVelocity(double velocityRadPerSec) {
    setState(VecBuilder.fill(velocityRadPerSec));
  }

  /**
   * Sets the flywheel's angular velocity.
   *
   * @param velocity The new angular velocity.
   */
  public void setAngularVelocity(AngularVelocity velocity) {
    setAngularVelocity(velocity.in(RadiansPerSecond));
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
   * Returns the moment of inertia in kilograms meters squared.
   *
   * @return The flywheel's moment of inertia.
   */
  public double getJKgMetersSquared() {
    return m_j.in(KilogramSquareMeters);
  }

  /**
   * Returns the moment of inertia.
   *
   * @return The flywheel's moment of inertia.
   */
  public MomentOfInertia getJ() {
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
   * Returns the flywheel's velocity in Radians Per Second.
   *
   * @return The flywheel's velocity in Radians Per Second.
   */
  public double getAngularVelocityRadPerSec() {
    return m_angularVelocity.in(RadiansPerSecond);
  }

  /**
   * Returns the flywheel's velocity in RPM.
   *
   * @return The flywheel's velocity in RPM.
   */
  public double getAngularVelocityRPM() {
    return m_angularVelocity.in(RPM);
  }

  /**
   * Returns the flywheel's velocity.
   *
   * @return The flywheel's velocity
   */
  public AngularVelocity getAngularVelocity() {
    return m_angularVelocity;
  }

  /**
   * Returns the flywheel's acceleration in Radians Per Second Squared.
   *
   * @return The flywheel's acceleration in Radians Per Second Squared.
   */
  public double getAngularAccelerationRadPerSecSq() {
    return m_angularAcceleration.in(RadiansPerSecondPerSecond);
  }

  /**
   * Returns the flywheel's acceleration.
   *
   * @return The flywheel's acceleration.
   */
  public AngularAcceleration getAngularAcceleration() {
    return m_angularAcceleration;
  }

  /**
   * Returns the flywheel's current draw in Amps.
   *
   * @return The flywheel's current draw in Amps.
   */
  public double getCurrentDrawAmps() {
    return m_currentDraw.in(Amps);
  }

  /**
   * Returns the flywheel's current draw.
   *
   * @return The flywheel's current draw.
   */
  public Current getCurrentDraw() {
    return m_currentDraw;
  }

  /**
   * Returns the flywheel's torque in Newton-Meters.
   *
   * @return The flywheel's torque in Newton-Meters.
   */
  public double getTorqueNewtonMeters() {
    return m_torque.in(NewtonMeters);
  }

  /**
   * Returns the flywheel's torque.
   *
   * @return The flywheel's torque.
   */
  public Torque getTorque() {
    return m_torque;
  }

  /**
   * Returns the voltage of the flywheel in volts.
   *
   * @return The flywheel's voltage in volts.
   */
  public double getVoltageVolts() {
    return m_voltage.in(Volts);
  }

  /**
   * Returns the voltage of the flywheel.
   *
   * @return The flywheel's voltage.
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
