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

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.NumericalIntegration;
import edu.wpi.first.math.system.plant.DCMotor;
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
import edu.wpi.first.units.measure.Torque;
import edu.wpi.first.units.measure.Voltage;

/** Represents a simulated elevator mechanism. */
public abstract class ElevatorSim extends LinearSystemSim<N2, N1, N2> {
  // Gearbox for the elevator.
  protected final DCMotor m_gearbox;

  // The gearing from the motors to the output.
  protected final double m_gearing;

  // The mass of the elevator carriage.
  private final Mass m_mass;

  // the drum radius of the elevator.
  protected final Distance m_drumRadius;

  // The min allowable height for the elevator.
  private final Distance m_minHeight;

  // The max allowable height for the elevator.
  private final Distance m_maxHeight;

  // The acceleration due to gravity.
  private final LinearAcceleration m_g;

  // The position of the elevator.
  private final MutDistance m_position = Meters.mutable(0.0);

  // The velocity of the elevator.
  private final MutLinearVelocity m_velocity = MetersPerSecond.mutable(0.0);

  // The acceleration of the elevator.
  private final MutLinearAcceleration m_acceleration = MetersPerSecondPerSecond.mutable(0.0);

  // The current draw of elevator.
  protected final MutCurrent m_currentDraw = Amps.mutable(0.0);

  // The voltage of the elevator.
  protected final MutVoltage m_voltage = Volts.mutable(0.0);

  // The torque on the elevator's drum.
  protected final MutTorque m_torque = NewtonMeters.mutable(0.0);

  // The force on the elevator's carriage.
  protected final MutForce m_force = Newtons.mutable(0.0);

  /**
   * Creates a simulated elevator mechanism.
   *
   * @param plant              The linear system that represents the
   *                           elevator.
   *                           This system can be created with
   *                           {@link edu.wpi.first.math.system.plant.LinearSystemId#createElevatorSystem(DCMotor, double,
   *                           double, double)}or {@link
   *                           edu.wpi.first.math.system.plant.LinearSystemId#identifyPositionSystem(double, double)}.
   *                           If
   *                           {@link edu.wpi.first.math.system.plant.LinearSystemId#identifyPositionSystem(double, double)}
   *                           is used, the distance unit must be meters.
   * @param gearbox            The type of and number of motors in the
   *                           elevator
   *                           gearbox.
   * @param gearing            The gearing from the motors to the output.
   * @param mass               The mass of the elevator's carriage.
   * @param drumRadius         The radius of the elevator's drum.
   * @param minHeight          The min allowable height of the elevator.
   * @param maxHeight          The max allowable height of the elevator.
   * @param g                  The acceleration due to gravity.
   * @param startingHeight     The starting height of the elevator.
   * @param measurementStdDevs The standard deviations of the measurements.
   *                           Can
   *                           be omitted if no
   *                           noise is desired. If present must have 1
   *                           element
   *                           for position.
   */
  @SuppressWarnings("this-escape")
  public ElevatorSim(
      LinearSystem<N2, N1, N2> plant,
      DCMotor gearbox,
      double gearing,
      Mass mass,
      Distance drumRadius,
      Distance minHeight,
      Distance maxHeight,
      LinearAcceleration g,
      Distance startingHeight,
      double... measurementStdDevs) {
    super(plant, measurementStdDevs);
    m_gearbox = gearbox;
    m_gearing = gearing;
    m_mass = mass;
    m_drumRadius = drumRadius;
    m_minHeight = minHeight;
    m_maxHeight = maxHeight;
    m_g = g;
    setState(startingHeight.in(Meters), 0);
  }

  /**
   * Sets the elevator's state. The new position will be limited between the
   * minimum and maximum
   * allowed heights.
   *
   * @param positionMeters          The new position in meters.
   * @param velocityMetersPerSecond New velocity in meters per second.
   */
  public void setState(double positionMeters, double velocityMetersPerSecond) {
    setState(
        VecBuilder.fill(
            MathUtil.clamp(positionMeters, m_minHeight.in(Meters), m_maxHeight.in(Meters)), velocityMetersPerSecond));
  }

  /**
   * Sets the elevator's state. The new position will be limited between the
   * minimum and maximum
   * allowed heights.
   *
   * @param position The new position.
   * @param velocity The new velocity.
   */
  public void setState(Distance position, LinearVelocity velocity) {
    setState(position.in(Meters), velocity.in(MetersPerSecond));
  }

  /**
   * Sets the elevator's position. The new position will be limited bewtween the
   * minimum and maximum allowed heights.
   *
   * @param positionMeters The new position in meters.
   */
  public void setPosition(double positionMeters) {
    setState(positionMeters, m_velocity.in(MetersPerSecond));
  }

  /**
   * Sets the elevator's position. The new position will be limited bewtween the
   * minimum and maximum allowed heights.
   *
   * @param position The new position.
   */
  public void setPosition(Distance position) {
    setPosition(position.in(Meters));
  }

  /**
   * Sets the elevator's velocity.
   *
   * @param velocityMetersPerSecond The new velocity in meters per second.
   */
  public void setVelocity(double velocityMetersPerSecond) {
    setState(m_position.in(Meters), velocityMetersPerSecond);
  }

  /**
   * Sets the elevator's velocity.
   *
   * @param velocity The new velocity.
   */
  public void setVelocity(LinearVelocity velocity) {
    setVelocity(velocity.in(MetersPerSecond));
  }

  /**
   * Returns whether the elevator would hit the lower limit.
   *
   * @param elevatorHeightMeters The elevator height in meters.
   * @return Whether the elevator would hit the lower limit.
   */
  public boolean wouldHitLowerLimit(double elevatorHeightMeters) {
    return elevatorHeightMeters <= this.m_minHeight.in(Meters);
  }

  /**
   * Returns whether the elevator would hit the lower limit.
   *
   * @param elevatorHeight The elevator height.
   * @return Whether the elevator would hit the lower limit.
   */
  public boolean wouldHitLowerLimit(Distance elevatorHeight) {
    return wouldHitLowerLimit(elevatorHeight.in(Meters));
  }

  /**
   * Returns whether the elevator would hit the upper limit.
   *
   * @param elevatorHeightMeters The elevator height in meters.
   * @return Whether the elevator would hit the upper limit.
   */
  public boolean wouldHitUpperLimit(double elevatorHeightMeters) {
    return elevatorHeightMeters >= this.m_maxHeight.in(Meters);
  }

  /**
   * Returns whether the elevator would hit the upper limit.
   *
   * @param elevatorHeight The elevator height.
   * @return Whether the elevator would hit the upper limit.
   */
  public boolean wouldHitUpperLimit(Distance elevatorHeight) {
    return wouldHitUpperLimit(elevatorHeight.in(Meters));
  }

  /**
   * Returns whether the elevator has hit the lower limit.
   *
   * @return Whether the elevator has hit the lower limit.
   */
  public boolean hasHitLowerLimit() {
    return wouldHitLowerLimit(m_position);
  }

  /**
   * Returns whether the elevator has hit the upper limit.
   *
   * @return Whether the elevator has hit the upper limit.
   */
  public boolean hasHitUpperLimit() {
    return wouldHitUpperLimit(m_position);
  }

  /**
   * Returns the gear ratio of the elevator's gear box.
   *
   * @return the elevator gear box's gear ratio.
   */
  public double getGearing() {
    return m_gearing;
  }

  /**
   * Returns the mass of the elevator's carriage in kilograms.
   *
   * @return the carriage mass of the elevator in kilograms.
   */
  public double getMassKilograms() {
    return m_mass.in(Kilograms);
  }

  /**
   * Returns the mass of the elevator's carriage.
   *
   * @return the mass of the elevator's carriage.
   */
  public Mass getMass() {
    return m_mass;
  }

  /**
   * Returns the drum radius of the elevator in meters.
   *
   * @return the drum radius of the elevator in meters.
   */
  public double getDrumRadiusMeters() {
    return m_drumRadius.in(Meters);
  }

  /**
   * Returns the drum radius of the elevator.
   *
   * @return the drum radius of the elevator.
   */
  public Distance getDrumRadius() {
    return m_drumRadius;
  }

  /**
   * Returns the gearbox for the elevator.
   *
   * @return The elevator's gearbox.
   */
  public DCMotor getGearbox() {
    return m_gearbox;
  }

  /**
   * Returns the acceleration due to gravity in meters per second squared.
   *
   * @return The acceleeration due to gravity in meters per second squared.
   */
  public double getGMetersPerSecondSquared() {
    return m_g.in(MetersPerSecondPerSecond);
  }

  /**
   * Returns the acceleration due to gravity.
   *
   * @return The acceleration due to gravity.
   */
  public LinearAcceleration getG() {
    return m_g;
  }

  /**
   * Returns the position of the elevator in meters.
   *
   * @return The position of the elevator in meters.
   */
  public double getPositionMeters() {
    return m_position.in(Meters);
  }

  /**
   * Returns the position of the elevator.
   *
   * @return The position of the elevator.
   */
  public Distance getPosition() {
    return m_position;
  }

  /**
   * Returns the velocity of the elevator in meters per second.
   *
   * @return The velocity of the elevator in meters per second.
   */
  public double getVelocityMetersPerSecond() {
    return m_velocity.in(MetersPerSecond);
  }

  /**
   * Returns the velocity of the elevator.
   *
   * @return The velocity of the elevator.
   */
  public LinearVelocity getVelocity() {
    return m_velocity;
  }

  /**
   * Returns the acceleration of the elevator in meters per second squared.
   *
   * @return The acceleration of the elevator in meters per second squared.
   */
  public double getAccelerationMetersPerSecondSquared() {
    return m_acceleration.in(MetersPerSecondPerSecond);
  }

  /**
   * Returns the acceleration of the elevator.
   *
   * @return The acceleration of the elevator.
   */
  public LinearAcceleration getAcceleration() {
    return m_acceleration;
  }

  /**
   * Returns the elevator's current draw in Amps.
   *
   * @return The elevator's current draw in Amps.
   */
  public double getCurrentDrawAmps() {
    return m_currentDraw.in(Amps);
  }

  /**
   * Returns the elevator's current draw.
   *
   * @return The elevator's current draw.
   */
  public Current getCurrentDraw() {
    return m_currentDraw;
  }

  /**
   * Returns the torque on the elevator's drum in Newton-Meters.
   *
   * @return The torque on the elevator's drum in Newton-Meters.
   */
  public double getTorqueNewtonMeters() {
    return m_torque.in(NewtonMeters);
  }

  /**
   * Returns the torque on the elevator's drum.
   *
   * @return The torque on the elevator's drum.
   */
  public Torque getTorque() {
    return m_torque;
  }

  /**
   * Returns the force on the elevator's carriage in Newtons
   *
   * @return The force on the elevator's carriage in Newtons.
   */
  public double getForceNewtons() {
    return m_force.in(Newtons);
  }

  /**
   * Returns the force on the elevator's carriage.
   *
   * @return The force on the elevator's carriage.
   */
  public Force getForce() {
    return m_force;
  }

  /**
   * Returns the voltage of the elevator in volts.
   *
   * @return The voltage of the elevator in volts.
   */
  public double getVoltageVolts() {
    return m_voltage.in(Volts);
  }

  /**
   * Returns the voltage of the elevator.
   *
   * @return The voltage of the elevator.
   */
  public Voltage getVoltage() {
    return m_voltage;
  }

  /**
   * Updates the state of the elevator.
   *
   * @param currentXhat The current state estimate.
   * @param u           The system inputs (voltage).
   * @param dtSeconds   The time difference between controller updates.
   */
  @Override
  protected Matrix<N2, N1> updateX(Matrix<N2, N1> currentXhat, Matrix<N1, N1> u, double dtSeconds) {
    // Calculate updated x-hat from Runge-Kutta.
    var updatedXhat = NumericalIntegration.rkdp(
        (x, _u) -> {
          Matrix<N2, N1> xdot = m_plant.getA().times(x).plus(m_plant.getB().times(_u));
          xdot = xdot.plus(VecBuilder.fill(0, m_g.in(MetersPerSecondPerSecond)));
          return xdot;
        },
        currentXhat,
        u,
        dtSeconds);

    // We check for collisions after updating x-hat.
    if (wouldHitLowerLimit(updatedXhat.get(0, 0))) {
      return VecBuilder.fill(m_minHeight.in(Meters), 0);
    }
    if (wouldHitUpperLimit(updatedXhat.get(0, 0))) {
      return VecBuilder.fill(m_maxHeight.in(Meters), 0);
    }
    return updatedXhat;
  }

  @Override
  public void update(double dtSeconds) {
    super.update(dtSeconds);
    m_position.mut_replace(getOutput(0), Meters);
    m_velocity.mut_replace(getOutput(1), MetersPerSecond);
    m_acceleration.mut_replace((m_plant.getA().times(m_x)).plus(m_plant.getB().times(m_u)).get(0, 0),
        MetersPerSecondPerSecond);
    // I = V / R - omega / (Kv * R)
    // Reductions are greater than 1, so a reduction of 10:1 would mean the motor is
    // spinning 10x faster than the output
    // v = r w, so w = v/r
    m_currentDraw.mut_replace(
        m_gearbox.getCurrent(m_x.get(1, 0) * m_gearing / 2 / Math.PI / m_drumRadius.in(Meters), m_u.get(0, 0))
            * Math.signum(m_u.get(0, 0)),
        Amps);
  }
}
