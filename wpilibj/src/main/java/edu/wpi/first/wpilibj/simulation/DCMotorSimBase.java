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
import edu.wpi.first.math.system.plant.DCMotor;
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

/** Represents a simulated DC motor mechanism. */
public abstract class DCMotorSimBase extends LinearSystemSim<N2, N1, N2> {
  // Gearbox for the DC motor.
  protected final DCMotor m_gearbox;

  // The gearing from the motors to the output.
  protected final double m_gearing;

  // The moment of inertia for the flywheel mechanism.
  private final MomentOfInertia m_j;

  // The angle of the system.
  private final MutAngle m_angle = Radians.mutable(0.0);

  // The angular velocity of the system.
  protected final MutAngularVelocity m_angularVelocity = RadiansPerSecond.mutable(0.0);

  // The angular acceleration of the system.
  private final MutAngularAcceleration m_angularAcceleration =
      RadiansPerSecondPerSecond.mutable(0.0);

  // The current draw of flywheel.
  protected final MutCurrent m_currentDraw = Amps.mutable(0.0);

  // The voltage of the flywheel.
  protected final MutVoltage m_voltage = Volts.mutable(0.0);

  // The torque on the flywheel.
  protected final MutTorque m_torque = NewtonMeters.mutable(0.0);

  /**
   * Creates a simulated DC motor mechanism.
   *
   * @param plant The linear system representing the DC motor. This system can be created with
   *     {@link edu.wpi.first.math.system.plant.LinearSystemId#createDCMotorSystem(DCMotor, double,
   *     double)} or {@link
   *     edu.wpi.first.math.system.plant.LinearSystemId#createDCMotorSystem(double, double)}. If
   *     {@link edu.wpi.first.math.system.plant.LinearSystemId#createDCMotorSystem(double, double)}
   *     is used, the distance unit must be radians.
   * @param gearbox The type of and number of motors in the DC motor gearbox.
   * @param gearing The gearing from the motors to the output.
   * @param J The moment of inertia for the flywheel mechanism.
   * @param measurementStdDevs The standard deviations of the measurements. Can be omitted if no
   *     noise is desired. If present must have 2 elements. The first element is for position. The
   *     second element is for velocity.
   */
  public DCMotorSimBase(
      LinearSystem<N2, N1, N2> plant,
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
   * Sets the state of the DC motor.
   *
   * @param angularPositionRad The new angular position in radians.
   * @param angularVelocityRadPerSec The new angualr velocity in radians per second.
   */
  public void setState(double angularPositionRad, double angularVelocityRadPerSec) {
    setState(VecBuilder.fill(angularPositionRad, angularVelocityRadPerSec));
  }

  /**
   * Sets the state of the DC motor.
   *
   * @param angularPosition The new angular position.
   * @param angularVelocity The new angular velocity.
   */
  public void setState(Angle angularPosition, AngularVelocity angularVelocity) {
    setState(angularPosition.in(Radians), angularVelocity.in(RadiansPerSecond));
  }

  /**
   * Sets the DC motor's angular position.
   *
   * @param angularPositionRad The new position in radians.
   */
  public void setAngle(double angularPositionRad) {
    setState(angularPositionRad, m_angularVelocity.in(RadiansPerSecond));
  }

  /**
   * Sets the DC motor's angular position.
   *
   * @param angularPosition The new angular position.
   */
  public void setAngle(Angle angularPosition) {
    setAngle(angularPosition.in(Radians));
  }

  /**
   * Sets the DC motor's angular velocity.
   *
   * @param angularVelocityRadPerSec The new velocity in radians per second.
   */
  public void setAngularVelocity(double angularVelocityRadPerSec) {
    setState(m_angle.in(Radians), angularVelocityRadPerSec);
  }

  /**
   * Sets the DC motor's angular velocity.
   *
   * @param angularVelocity The new velocity.
   */
  public void setAngularVelocity(AngularVelocity angularVelocity) {
    setAngularVelocity(angularVelocity.in(RadiansPerSecond));
  }

  /**
   * Returns the gear ratio of the DC motor.
   *
   * @return the DC motor's gear ratio.
   */
  public double getGearing() {
    return m_gearing;
  }

  /**
   * Returns the moment of inertia in kilograms meters squared.
   *
   * @return The DC motor's moment of inertia in kilograms meters squared.
   */
  public double getJKgMetersSquared() {
    return m_j.in(KilogramSquareMeters);
  }

  /**
   * Returns the moment of inertia.
   *
   * @return The DC motor's moment of inertia.
   */
  public MomentOfInertia getJ() {
    return m_j;
  }

  /**
   * Returns the gearbox for the DC motor.
   *
   * @return The DC motor's gearbox.
   */
  public DCMotor getGearbox() {
    return m_gearbox;
  }

  /**
   * Returns the DC motor's position.
   *
   * @return The DC motor's position.
   */
  public double getAngularPositionRad() {
    return m_angle.in(Radians);
  }

  /**
   * Returns the DC motor's position in rotations.
   *
   * @return The DC motor's position in rotations.
   */
  public double getAngularPositionRotations() {
    return m_angle.in(Rotations);
  }

  /**
   * Returns the DC motor's position.
   *
   * @return The DC motor's position
   */
  public Angle getAngularPosition() {
    return m_angle;
  }

  /**
   * Returns the DC motor's velocity.
   *
   * @return The DC motor's velocity.
   */
  public double getAngularVelocityRadPerSec() {
    return m_angularVelocity.in(RadiansPerSecond);
  }

  /**
   * Returns the DC motor's velocity in RPM.
   *
   * @return The DC motor's velocity in RPM.
   */
  public double getAngularVelocityRPM() {
    return m_angularVelocity.in(RPM);
  }

  /**
   * Returns the DC motor's velocity.
   *
   * @return The DC motor's velocity
   */
  public AngularVelocity getAngularVelocity() {
    return m_angularVelocity;
  }

  /**
   * Returns the DC motor's acceleration in Radians Per Second Squared.
   *
   * @return The DC motor's acceleration in Radians Per Second Squared.
   */
  public double getAngularAccelerationRadPerSecSq() {
    return m_angularAcceleration.in(RadiansPerSecondPerSecond);
  }

  /**
   * Returns the DC motor's acceleration.
   *
   * @return The DC motor's acceleration.
   */
  public AngularAcceleration getAngularAcceleration() {
    return m_angularAcceleration;
  }

  /**
   * Returns the DC motor's current draw in Amps.
   *
   * @return The DC motor's current draw in Amps.
   */
  public double getCurrentDrawAmps() {
    return m_currentDraw.in(Amps);
  }

  /**
   * Returns the DC motor's current draw.
   *
   * @return The DC motor's current draw.
   */
  public Current getCurrentDraw() {
    return m_currentDraw;
  }

  /**
   * Returns the DC motor's torque in Newton-Meters.
   *
   * @return The DC motor's torque in Newton-Meters.
   */
  public double getTorqueNewtonMeters() {
    return m_torque.in(NewtonMeters);
  }

  /**
   * Returns the DC motor's torque.
   *
   * @return The DC motor's torque.
   */
  public Torque getTorque() {
    return m_torque;
  }

  /**
   * Returns the voltage of the DC motor in volts.
   *
   * @return The DC motor's voltage in volts.
   */
  public double getVoltageVolts() {
    return m_voltage.in(Volts);
  }

  /**
   * Returns the voltage of the DC motor.
   *
   * @return The DC motor's voltage.
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
