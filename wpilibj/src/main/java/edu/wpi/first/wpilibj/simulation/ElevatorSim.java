// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.NumericalIntegration;
import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.math.system.plant.LinearSystemId;
import edu.wpi.first.wpilibj.RobotController;

/** Represents a simulated elevator mechanism. */
public class ElevatorSim extends LinearSystemSim<N2, N1, N2> {
  // Gearbox for the elevator.
  private final DCMotor m_gearbox;

  // The min allowable height for the elevator.
  private final double m_minHeight;

  // The max allowable height for the elevator.
  private final double m_maxHeight;

  // Whether the simulator should simulate gravity.
  private final boolean m_simulateGravity;

  /**
   * Creates a simulated elevator mechanism.
   *
   * @param plant The linear system that represents the elevator. This system can be created with
   *     {@link edu.wpi.first.math.system.plant.LinearSystemId#createElevatorSystem(DCMotor, double,
   *     double, double)}.
   * @param gearbox The type of and number of motors in the elevator gearbox.
   * @param minHeight The min allowable height of the elevator in meters.
   * @param maxHeight The max allowable height of the elevator in meters.
   * @param simulateGravity Whether gravity should be simulated or not.
   * @param startingHeight The starting height of the elevator in meters.
   * @param measurementStdDevs The standard deviations of the measurements. Can be omitted if no
   *     noise is desired. If present must have 1 element for position.
   */
  @SuppressWarnings("this-escape")
  public ElevatorSim(
      LinearSystem<N2, N1, N2> plant,
      DCMotor gearbox,
      double minHeight,
      double maxHeight,
      boolean simulateGravity,
      double startingHeight,
      double... measurementStdDevs) {
    super(plant, measurementStdDevs);
    m_gearbox = gearbox;
    m_minHeight = minHeight;
    m_maxHeight = maxHeight;
    m_simulateGravity = simulateGravity;

    setState(startingHeight, 0);
  }

  /**
   * Creates a simulated elevator mechanism.
   *
   * @param kV The velocity gain.
   * @param kA The acceleration gain.
   * @param gearbox The type of and number of motors in the elevator gearbox.
   * @param minHeight The min allowable height of the elevator in meters.
   * @param maxHeight The max allowable height of the elevator in meters.
   * @param simulateGravity Whether gravity should be simulated or not.
   * @param startingHeight The starting height of the elevator in meters.
   * @param measurementStdDevs The standard deviations of the measurements. Can be omitted if no
   *     noise is desired. If present must have 1 element for position.
   */
  public ElevatorSim(
      double kV,
      double kA,
      DCMotor gearbox,
      double minHeight,
      double maxHeight,
      boolean simulateGravity,
      double startingHeight,
      double... measurementStdDevs) {
    this(
        LinearSystemId.identifyPositionSystem(kV, kA),
        gearbox,
        minHeight,
        maxHeight,
        simulateGravity,
        startingHeight,
        measurementStdDevs);
  }

  /**
   * Creates a simulated elevator mechanism.
   *
   * @param gearbox The type of and number of motors in the elevator gearbox.
   * @param gearing The gearing of the elevator (numbers greater than 1 represent reductions).
   * @param carriageMass The mass of the elevator carriage in kg.
   * @param drumRadius The radius of the drum that the elevator spool is wrapped around in meters.
   * @param minHeight The min allowable height of the elevator in meters.
   * @param maxHeight The max allowable height of the elevator in meters.
   * @param simulateGravity Whether gravity should be simulated or not.
   * @param startingHeight The starting height of the elevator in meters.
   * @param measurementStdDevs The standard deviations of the measurements. Can be omitted if no
   *     noise is desired. If present must have 1 element for position.
   */
  public ElevatorSim(
      DCMotor gearbox,
      double gearing,
      double carriageMass,
      double drumRadius,
      double minHeight,
      double maxHeight,
      boolean simulateGravity,
      double startingHeight,
      double... measurementStdDevs) {
    this(
        LinearSystemId.createElevatorSystem(gearbox, carriageMass, drumRadius, gearing),
        gearbox,
        minHeight,
        maxHeight,
        simulateGravity,
        startingHeight,
        measurementStdDevs);
  }

  /**
   * Sets the elevator's state. The new position will be limited between the minimum and maximum
   * allowed heights.
   *
   * @param position The new position in meters.
   * @param velocity New velocity in meters per second.
   */
  public final void setState(double position, double velocity) {
    setState(VecBuilder.fill(MathUtil.clamp(position, m_minHeight, m_maxHeight), velocity));
  }

  /**
   * Returns whether the elevator would hit the lower limit.
   *
   * @param elevatorHeight The elevator height in meters.
   * @return Whether the elevator would hit the lower limit.
   */
  public boolean wouldHitLowerLimit(double elevatorHeight) {
    return elevatorHeight <= this.m_minHeight;
  }

  /**
   * Returns whether the elevator would hit the upper limit.
   *
   * @param elevatorHeight The elevator height in meters.
   * @return Whether the elevator would hit the upper limit.
   */
  public boolean wouldHitUpperLimit(double elevatorHeight) {
    return elevatorHeight >= this.m_maxHeight;
  }

  /**
   * Returns whether the elevator has hit the lower limit.
   *
   * @return Whether the elevator has hit the lower limit.
   */
  public boolean hasHitLowerLimit() {
    return wouldHitLowerLimit(getPosition());
  }

  /**
   * Returns whether the elevator has hit the upper limit.
   *
   * @return Whether the elevator has hit the upper limit.
   */
  public boolean hasHitUpperLimit() {
    return wouldHitUpperLimit(getPosition());
  }

  /**
   * Returns the position of the elevator.
   *
   * @return The position of the elevator in meters.
   */
  public double getPosition() {
    return getOutput(0);
  }

  /**
   * Returns the velocity of the elevator.
   *
   * @return The velocity of the elevator in meters per second.
   */
  public double getVelocity() {
    return getOutput(1);
  }

  /**
   * Returns the elevator current draw.
   *
   * @return The elevator current draw in amps.
   */
  public double getCurrentDraw() {
    // I = V / R - omega / (Kv * R)
    // Reductions are greater than 1, so a reduction of 10:1 would mean the motor is
    // spinning 10x faster than the output
    // v = r w, so w = v/r
    double kA = 1 / m_plant.getB().get(1, 0);
    double kV = -m_plant.getA().get(1, 1) * kA;
    double motorVelocity = m_x.get(1, 0) * kV * m_gearbox.Kv;
    var appliedVoltage = m_u.get(0, 0);
    return m_gearbox.getCurrent(motorVelocity, appliedVoltage) * Math.signum(appliedVoltage);
  }

  /**
   * Sets the input voltage for the elevator.
   *
   * @param volts The input voltage.
   */
  public void setInputVoltage(double volts) {
    setInput(volts);
    clampInput(RobotController.getBatteryVoltage());
  }

  /**
   * Updates the state of the elevator.
   *
   * @param currentXhat The current state estimate.
   * @param u The system inputs (voltage).
   * @param dt The time difference between controller updates in seconds.
   */
  @Override
  protected Matrix<N2, N1> updateX(Matrix<N2, N1> currentXhat, Matrix<N1, N1> u, double dt) {
    // Calculate updated x-hat from Runge-Kutta.
    var updatedXhat =
        NumericalIntegration.rkdp(
            (x, _u) -> {
              Matrix<N2, N1> xdot = m_plant.getA().times(x).plus(m_plant.getB().times(_u));
              if (m_simulateGravity) {
                xdot = xdot.plus(VecBuilder.fill(0, -9.8));
              }
              return xdot;
            },
            currentXhat,
            u,
            dt);

    // We check for collisions after updating x-hat.
    if (wouldHitLowerLimit(updatedXhat.get(0, 0))) {
      return VecBuilder.fill(m_minHeight, 0);
    }
    if (wouldHitUpperLimit(updatedXhat.get(0, 0))) {
      return VecBuilder.fill(m_maxHeight, 0);
    }
    return updatedXhat;
  }
}
