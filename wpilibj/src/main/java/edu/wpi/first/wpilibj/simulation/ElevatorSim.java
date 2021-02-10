// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.wpilibj.system.LinearSystem;
import edu.wpi.first.wpilibj.system.NumericalIntegration;
import edu.wpi.first.wpilibj.system.plant.DCMotor;
import edu.wpi.first.wpilibj.system.plant.LinearSystemId;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.VecBuilder;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N2;

/** Represents a simulated elevator mechanism. */
public class ElevatorSim extends LinearSystemSim<N2, N1, N1> {
  // Gearbox for the elevator.
  private final DCMotor m_gearbox;

  // Gearing between the motors and the output.
  private final double m_gearing;

  // The radius of the drum that the elevator spool is wrapped around.
  private final double m_drumRadius;

  // The min allowable height for the elevator.
  private final double m_minHeight;

  // The max allowable height for the elevator.
  private final double m_maxHeight;

  /**
   * Creates a simulated elevator mechanism.
   *
   * @param plant The linear system that represents the elevator.
   * @param gearbox The type of and number of motors in the elevator gearbox.
   * @param gearing The gearing of the elevator (numbers greater than 1 represent reductions).
   * @param drumRadiusMeters The radius of the drum that the elevator spool is wrapped around.
   * @param minHeightMeters The min allowable height of the elevator.
   * @param maxHeightMeters The max allowable height of the elevator.
   */
  public ElevatorSim(
      LinearSystem<N2, N1, N1> plant,
      DCMotor gearbox,
      double gearing,
      double drumRadiusMeters,
      double minHeightMeters,
      double maxHeightMeters) {
    this(plant, gearbox, gearing, drumRadiusMeters, minHeightMeters, maxHeightMeters, null);
  }

  /**
   * Creates a simulated elevator mechanism.
   *
   * @param plant The linear system that represents the elevator.
   * @param gearbox The type of and number of motors in the elevator gearbox.
   * @param gearing The gearing of the elevator (numbers greater than 1 represent reductions).
   * @param drumRadiusMeters The radius of the drum that the elevator spool is wrapped around.
   * @param minHeightMeters The min allowable height of the elevator.
   * @param maxHeightMeters The max allowable height of the elevator.
   * @param measurementStdDevs The standard deviations of the measurements.
   */
  public ElevatorSim(
      LinearSystem<N2, N1, N1> plant,
      DCMotor gearbox,
      double gearing,
      double drumRadiusMeters,
      double minHeightMeters,
      double maxHeightMeters,
      Matrix<N1, N1> measurementStdDevs) {
    super(plant, measurementStdDevs);
    m_gearbox = gearbox;
    m_gearing = gearing;
    m_drumRadius = drumRadiusMeters;
    m_minHeight = minHeightMeters;
    m_maxHeight = maxHeightMeters;
  }

  /**
   * Creates a simulated elevator mechanism.
   *
   * @param gearbox The type of and number of motors in the elevator gearbox.
   * @param gearing The gearing of the elevator (numbers greater than 1 represent reductions).
   * @param carriageMassKg The mass of the elevator carriage.
   * @param drumRadiusMeters The radius of the drum that the elevator spool is wrapped around.
   * @param minHeightMeters The min allowable height of the elevator.
   * @param maxHeightMeters The max allowable height of the elevator.
   */
  public ElevatorSim(
      DCMotor gearbox,
      double gearing,
      double carriageMassKg,
      double drumRadiusMeters,
      double minHeightMeters,
      double maxHeightMeters) {
    this(
        gearbox, gearing, carriageMassKg, drumRadiusMeters, minHeightMeters, maxHeightMeters, null);
  }

  /**
   * Creates a simulated elevator mechanism.
   *
   * @param gearbox The type of and number of motors in the elevator gearbox.
   * @param gearing The gearing of the elevator (numbers greater than 1 represent reductions).
   * @param carriageMassKg The mass of the elevator carriage.
   * @param drumRadiusMeters The radius of the drum that the elevator spool is wrapped around.
   * @param minHeightMeters The min allowable height of the elevator.
   * @param maxHeightMeters The max allowable height of the elevator.
   * @param measurementStdDevs The standard deviations of the measurements.
   */
  public ElevatorSim(
      DCMotor gearbox,
      double gearing,
      double carriageMassKg,
      double drumRadiusMeters,
      double minHeightMeters,
      double maxHeightMeters,
      Matrix<N1, N1> measurementStdDevs) {
    super(
        LinearSystemId.createElevatorSystem(gearbox, carriageMassKg, drumRadiusMeters, gearing),
        measurementStdDevs);
    m_gearbox = gearbox;
    m_gearing = gearing;
    m_drumRadius = drumRadiusMeters;
    m_minHeight = minHeightMeters;
    m_maxHeight = maxHeightMeters;
  }

  /**
   * Returns whether the elevator would hit the lower limit.
   *
   * @param elevatorHeightMeters The elevator height.
   * @return Whether the elevator would hit the lower limit.
   */
  public boolean wouldHitLowerLimit(double elevatorHeightMeters) {
    return elevatorHeightMeters < this.m_minHeight;
  }

  /**
   * Returns whether the elevator would hit the upper limit.
   *
   * @param elevatorHeightMeters The elevator height.
   * @return Whether the elevator would hit the upper limit.
   */
  public boolean wouldHitUpperLimit(double elevatorHeightMeters) {
    return elevatorHeightMeters > this.m_maxHeight;
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
   * Returns the position of the elevator.
   *
   * @return The position of the elevator.
   */
  public double getPositionMeters() {
    return getOutput(0);
  }

  /**
   * Returns the velocity of the elevator.
   *
   * @return The velocity of the elevator.
   */
  public double getVelocityMetersPerSecond() {
    return m_x.get(1, 0);
  }

  /**
   * Returns the elevator current draw.
   *
   * @return The elevator current draw.
   */
  @Override
  public double getCurrentDrawAmps() {
    // I = V / R - omega / (Kv * R)
    // Reductions are greater than 1, so a reduction of 10:1 would mean the motor is
    // spinning 10x faster than the output
    // v = r w, so w = v/r
    double motorVelocityRadPerSec = getVelocityMetersPerSecond() / m_drumRadius * m_gearing;
    var appliedVoltage = m_u.get(0, 0);
    return m_gearbox.getCurrent(motorVelocityRadPerSec, appliedVoltage)
        * Math.signum(appliedVoltage);
  }

  /**
   * Sets the input voltage for the elevator.
   *
   * @param volts The input voltage.
   */
  public void setInputVoltage(double volts) {
    setInput(volts);
  }

  /**
   * Updates the state of the elevator.
   *
   * @param currentXhat The current state estimate.
   * @param u The system inputs (voltage).
   * @param dtSeconds The time difference between controller updates.
   */
  @SuppressWarnings({"ParameterName", "LambdaParameterName"})
  @Override
  protected Matrix<N2, N1> updateX(Matrix<N2, N1> currentXhat, Matrix<N1, N1> u, double dtSeconds) {
    // Calculate updated x-hat from Runge-Kutta.
    var updatedXhat =
        NumericalIntegration.rkf45(
            (x, u_) ->
                (m_plant.getA().times(x))
                    .plus(m_plant.getB().times(u_))
                    .plus(VecBuilder.fill(0, -9.8)),
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
}
