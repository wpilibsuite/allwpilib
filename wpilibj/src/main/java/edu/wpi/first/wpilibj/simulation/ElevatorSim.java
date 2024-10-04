// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static edu.wpi.first.units.Units.Meters;
import static edu.wpi.first.units.Units.MetersPerSecond;
import static edu.wpi.first.units.Units.MetersPerSecondPerSecond;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.NumericalIntegration;
import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.units.measure.Distance;
import edu.wpi.first.units.measure.LinearAcceleration;
import edu.wpi.first.units.measure.LinearVelocity;
import edu.wpi.first.units.measure.MutDistance;
import edu.wpi.first.units.measure.MutLinearAcceleration;
import edu.wpi.first.units.measure.MutLinearVelocity;

/** Represents a simulated elevator mechanism. */
public abstract class ElevatorSim extends LinearSystemSim<N2, N1, N2> {
  // The acceleration due to gravity in meters per second squared.
  private final double m_gMetersPerSecondSquared;

  // Gearbox for the elevator.
  private final DCMotor m_gearbox;

  // The gearing from the motors to the output.
  private final double m_gearing;

  // The mass of the elevator carriage in kilograms.
  private final double m_massKg;

  // the drum radius of the elevator in meters.
  private final double m_drumRadiusMeters;

  // The min allowable height for the elevator.
  private final double m_minHeight;

  // The max allowable height for the elevator.
  private final double m_maxHeight;

  // Whether the simulator should simulate gravity.
  private final boolean m_simulateGravity;

  // The angle of the system.
  private final MutDistance m_position = Meters.mutable(0.0);

  // The angular velocity of the system.
  private final MutLinearVelocity m_velocity = MetersPerSecond.mutable(0.0);

  // The angular acceleration of the system.
  private final MutLinearAcceleration m_acceleration = MetersPerSecondPerSecond.mutable(0.0);

  /**
   * Creates a simulated elevator mechanism.
   *
   * @param plant                   The linear system that represents the
   *                                elevator.
   *                                This system can be created with
   *                                {@link edu.wpi.first.math.system.plant.LinearSystemId#createElevatorSystem(DCMotor, double,
   *                                double, double)}.
   * @param gMetersPerSecondSquared The acceleration due to gravity in meters per
   *                                second squared.
   * @param gearbox                 The type of and number of motors in the
   *                                elevator
   *                                gearbox.
   * @param minHeightMeters         The min allowable height of the elevator.
   * @param maxHeightMeters         The max allowable height of the elevator.
   * @param simulateGravity         Whether gravity should be simulated or not.
   * @param startingHeightMeters    The starting height of the elevator.
   * @param measurementStdDevs      The standard deviations of the measurements.
   *                                Can
   *                                be omitted if no
   *                                noise is desired. If present must have 1
   *                                element
   *                                for position.
   */
  @SuppressWarnings("this-escape")
  public ElevatorSim(
      LinearSystem<N2, N1, N2> plant,
      double gMetersPerSecondSquared,
      DCMotor gearbox,
      double gearing,
      double massKg,
      double drumRadiusMeters,
      double minHeightMeters,
      double maxHeightMeters,
      boolean simulateGravity,
      double startingHeightMeters,
      double... measurementStdDevs) {
    super(plant, measurementStdDevs);
    m_gMetersPerSecondSquared = gMetersPerSecondSquared;
    m_gearbox = gearbox;
    m_gearing = gearing;
    m_massKg = massKg;
    m_drumRadiusMeters = drumRadiusMeters;
    m_minHeight = minHeightMeters;
    m_maxHeight = maxHeightMeters;
    m_simulateGravity = simulateGravity;
    setState(startingHeightMeters, 0);
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
            MathUtil.clamp(positionMeters, m_minHeight, m_maxHeight), velocityMetersPerSecond));
  }

  /**
   * Sets the elevator's position. The new position will be limited bewtween the
   * minimum and maximum allowed heights.
   * 
   * @param positionMeters The new position in meters.
   */
  public void setPosition(double positionMeters) {
    setState(
        VecBuilder.fill(
            MathUtil.clamp(positionMeters, m_minHeight, m_maxHeight), getVelocityMetersPerSecond()));
  }

  /**
   * Sets the elevator's velocity.
   * 
   * @param velocityMetersPerSecond The new velocity in meters per second.
   */
  public void setVelocity(double velocityMetersPerSecond) {
    setState(getPositionMeters(), velocityMetersPerSecond);
  }

  /**
   * Returns whether the elevator would hit the lower limit.
   *
   * @param elevatorHeightMeters The elevator height.
   * @return Whether the elevator would hit the lower limit.
   */
  public boolean wouldHitLowerLimit(double elevatorHeightMeters) {
    return elevatorHeightMeters <= this.m_minHeight;
  }

  /**
   * Returns whether the elevator would hit the upper limit.
   *
   * @param elevatorHeightMeters The elevator height.
   * @return Whether the elevator would hit the upper limit.
   */
  public boolean wouldHitUpperLimit(double elevatorHeightMeters) {
    return elevatorHeightMeters >= this.m_maxHeight;
  }

  /**
   * Returns whether the elevator has hit the lower limit.
   *
   * @return Whether the elevator has hit the lower limit.
   */
  public boolean hasHitLowerLimit() {
    return wouldHitLowerLimit(getPositionMeters());
  }

  /**
   * Returns whether the elevator has hit the upper limit.
   *
   * @return Whether the elevator has hit the upper limit.
   */
  public boolean hasHitUpperLimit() {
    return wouldHitUpperLimit(getPositionMeters());
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
   * Returns the mass of the elevator's carriage.
   * 
   * @return the carriage mass of the elevator.
   */
  public double getMassKg() {
    return m_massKg;
  }

  /**
   * Returns the drum radius of the elevator.
   * 
   * @return the drum radius of the elevator.
   */
  public double getDrumRadiusMeters() {
    return m_drumRadiusMeters;
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
   * Returns the position of the elevator.
   *
   * @return The position of the elevator.
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
   * Returns the velocity of the elevator.
   *
   * @return The velocity of the elevator.
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
   * Returns the acceleration of the elevator.
   * 
   * @return The acceleration of the elevator.
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
   * Returns the elevator current draw.
   *
   * @return The elevator current draw.
   */
  public double getCurrentDrawAmps() {
    // I = V / R - omega / (Kv * R)
    // Reductions are greater than 1, so a reduction of 10:1 would mean the motor is
    // spinning 10x faster than the output
    // v = r w, so w = v/r
    double kA = 1 / m_plant.getB().get(1, 0);
    double kV = -m_plant.getA().get(1, 1) * kA;
    double motorVelocityRadPerSec = m_x.get(1, 0) * kV * m_gearbox.KvRadPerSecPerVolt;
    var appliedVoltage = m_u.get(0, 0);
    return m_gearbox.getCurrent(motorVelocityRadPerSec, appliedVoltage)
        * Math.signum(appliedVoltage);
  }

  /**
   * Returns the torque on the elevator's drum in Newton-Meters.
   * 
   * @return The torque on the elevator's drum in Newton-Meters.
   */
  public abstract double getTorqueNewtonMeters();

  /**
   * Returns the force on the elevator's carriage in Newtons
   * 
   * @return The force on the elevator's carriage in Newtons.
   */
  public double getForceNewtons() {
    return getTorqueNewtonMeters() / m_drumRadiusMeters;
  }

  /**
   * Returns the voltage of the DC motor.
   *
   * @return The DC motor's voltage.
   */
  public abstract double getVoltage();

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
          if (m_simulateGravity) {
            xdot = xdot.plus(VecBuilder.fill(0, m_gMetersPerSecondSquared));
          }
          return xdot;
        },
        currentXhat,
        u,
        dtSeconds);

    // We check for collisions after updating x-hat.
    if (wouldHitLowerLimit(updatedXhat.get(0, 0))) {
      return VecBuilder.fill(m_minHeight, 0);
    }
    if (wouldHitUpperLimit(updatedXhat.get(0, 0))) {
      return VecBuilder.fill(m_maxHeight, 0);
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
  }
}
