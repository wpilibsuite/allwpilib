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

/** Represents a simulated single jointed arm mechanism. */
public class SingleJointedArmSim extends LinearSystemSim<N2, N1, N1> {
  // The gearbox for the arm.
  private final DCMotor m_gearbox;

  // The gearing between the motors and the output.
  private final double m_gearing;

  // The length of the arm.
  @SuppressWarnings("MemberName")
  private final double m_r;

  // The minimum angle that the arm is capable of.
  private final double m_minAngle;

  // The maximum angle that the arm is capable of.
  private final double m_maxAngle;

  // The mass of the arm.
  private final double m_armMass;

  // Whether the simulator should simulate gravity.
  private final boolean m_simulateGravity;

  /**
   * Creates a simulated arm mechanism.
   *
   * @param plant The linear system that represents the arm.
   * @param gearbox The type of and number of motors in the arm gearbox.
   * @param gearing The gearing of the arm (numbers greater than 1 represent reductions).
   * @param armLengthMeters The length of the arm.
   * @param minAngleRads The minimum angle that the arm is capable of.
   * @param maxAngleRads The maximum angle that the arm is capable of.
   * @param armMassKg The mass of the arm.
   * @param simulateGravity Whether gravity should be simulated or not.
   */
  public SingleJointedArmSim(
      LinearSystem<N2, N1, N1> plant,
      DCMotor gearbox,
      double gearing,
      double armLengthMeters,
      double minAngleRads,
      double maxAngleRads,
      double armMassKg,
      boolean simulateGravity) {
    this(
        plant,
        gearbox,
        gearing,
        armLengthMeters,
        minAngleRads,
        maxAngleRads,
        armMassKg,
        simulateGravity,
        null);
  }

  /**
   * Creates a simulated arm mechanism.
   *
   * @param plant The linear system that represents the arm.
   * @param gearbox The type of and number of motors in the arm gearbox.
   * @param gearing The gearing of the arm (numbers greater than 1 represent reductions).
   * @param armLengthMeters The length of the arm.
   * @param minAngleRads The minimum angle that the arm is capable of.
   * @param maxAngleRads The maximum angle that the arm is capable of.
   * @param armMassKg The mass of the arm.
   * @param simulateGravity Whether gravity should be simulated or not.
   * @param measurementStdDevs The standard deviations of the measurements.
   */
  public SingleJointedArmSim(
      LinearSystem<N2, N1, N1> plant,
      DCMotor gearbox,
      double gearing,
      double armLengthMeters,
      double minAngleRads,
      double maxAngleRads,
      double armMassKg,
      boolean simulateGravity,
      Matrix<N1, N1> measurementStdDevs) {
    super(plant, measurementStdDevs);
    m_gearbox = gearbox;
    m_gearing = gearing;
    m_r = armLengthMeters;
    m_minAngle = minAngleRads;
    m_maxAngle = maxAngleRads;
    m_armMass = armMassKg;
    m_simulateGravity = simulateGravity;
  }

  /**
   * Creates a simulated arm mechanism.
   *
   * @param gearbox The type of and number of motors in the arm gearbox.
   * @param gearing The gearing of the arm (numbers greater than 1 represent reductions).
   * @param jKgMetersSquared The moment of inertia of the arm, can be calculated from CAD software.
   * @param armLengthMeters The length of the arm.
   * @param minAngleRads The minimum angle that the arm is capable of.
   * @param maxAngleRads The maximum angle that the arm is capable of.
   * @param armMassKg The mass of the arm.
   * @param simulateGravity Whether gravity should be simulated or not.
   */
  @SuppressWarnings("ParameterName")
  public SingleJointedArmSim(
      DCMotor gearbox,
      double gearing,
      double jKgMetersSquared,
      double armLengthMeters,
      double minAngleRads,
      double maxAngleRads,
      double armMassKg,
      boolean simulateGravity) {
    this(
        gearbox,
        gearing,
        jKgMetersSquared,
        armLengthMeters,
        minAngleRads,
        maxAngleRads,
        armMassKg,
        simulateGravity,
        null);
  }

  /**
   * Creates a simulated arm mechanism.
   *
   * @param gearbox The type of and number of motors in the arm gearbox.
   * @param gearing The gearing of the arm (numbers greater than 1 represent reductions).
   * @param jKgMetersSquared The moment of inertia of the arm; can be calculated from CAD software.
   * @param armLengthMeters The length of the arm.
   * @param minAngleRads The minimum angle that the arm is capable of.
   * @param maxAngleRads The maximum angle that the arm is capable of.
   * @param armMassKg The mass of the arm.
   * @param simulateGravity Whether gravity should be simulated or not.
   * @param measurementStdDevs The standard deviations of the measurements.
   */
  @SuppressWarnings("ParameterName")
  public SingleJointedArmSim(
      DCMotor gearbox,
      double gearing,
      double jKgMetersSquared,
      double armLengthMeters,
      double minAngleRads,
      double maxAngleRads,
      double armMassKg,
      boolean simulateGravity,
      Matrix<N1, N1> measurementStdDevs) {
    super(
        LinearSystemId.createSingleJointedArmSystem(gearbox, jKgMetersSquared, gearing),
        measurementStdDevs);
    m_gearbox = gearbox;
    m_gearing = gearing;
    m_r = armLengthMeters;
    m_minAngle = minAngleRads;
    m_maxAngle = maxAngleRads;
    m_armMass = armMassKg;
    m_simulateGravity = simulateGravity;
  }

  /**
   * Returns whether the arm would hit the lower limit.
   *
   * @param currentAngleRads The current arm height.
   * @return Whether the arm would hit the lower limit.
   */
  public boolean wouldHitLowerLimit(double currentAngleRads) {
    return currentAngleRads < this.m_minAngle;
  }

  /**
   * Returns whether the arm would hit the upper limit.
   *
   * @param currentAngleRads The current arm height.
   * @return Whether the arm would hit the upper limit.
   */
  public boolean wouldHitUpperLimit(double currentAngleRads) {
    return currentAngleRads > this.m_maxAngle;
  }

  /**
   * Returns whether the arm has hit the lower limit.
   *
   * @return Whether the arm has hit the lower limit.
   */
  public boolean hasHitLowerLimit() {
    return wouldHitLowerLimit(getAngleRads());
  }

  /**
   * Returns whether the arm has hit the upper limit.
   *
   * @return Whether the arm has hit the upper limit.
   */
  public boolean hasHitUpperLimit() {
    return wouldHitUpperLimit(getAngleRads());
  }

  /**
   * Returns the current arm angle.
   *
   * @return The current arm angle.
   */
  public double getAngleRads() {
    return m_y.get(0, 0);
  }

  /**
   * Returns the current arm velocity.
   *
   * @return The current arm velocity.
   */
  public double getVelocityRadPerSec() {
    return m_x.get(1, 0);
  }

  /**
   * Returns the arm current draw.
   *
   * @return The aram current draw.
   */
  @Override
  public double getCurrentDrawAmps() {
    // Reductions are greater than 1, so a reduction of 10:1 would mean the motor is
    // spinning 10x faster than the output
    var motorVelocity = m_x.get(1, 0) * m_gearing;
    return m_gearbox.getCurrent(motorVelocity, m_u.get(0, 0)) * Math.signum(m_u.get(0, 0));
  }

  /**
   * Sets the input voltage for the arm.
   *
   * @param volts The input voltage.
   */
  public void setInputVoltage(double volts) {
    setInput(volts);
  }

  /**
   * Calculates a rough estimate of the moment of inertia of an arm given its length and mass.
   *
   * @param lengthMeters The length of the arm.
   * @param massKg The mass of the arm.
   * @return The calculated moment of inertia.
   */
  public static double estimateMOI(double lengthMeters, double massKg) {
    return 1.0 / 3.0 * massKg * lengthMeters * lengthMeters;
  }

  /**
   * Updates the state of the arm.
   *
   * @param currentXhat The current state estimate.
   * @param u The system inputs (voltage).
   * @param dtSeconds The time difference between controller updates.
   */
  @Override
  @SuppressWarnings({"ParameterName", "LambdaParameterName"})
  protected Matrix<N2, N1> updateX(Matrix<N2, N1> currentXhat, Matrix<N1, N1> u, double dtSeconds) {
    // Horizontal case:
    // Torque = F * r = I * alpha
    // alpha = F * r / I
    // Since F = mg,
    // alpha = m * g * r / I
    // Finally, multiply RHS by cos(theta) to account for the arm angle
    // This acceleration is added to the linear system dynamics x-dot = Ax + Bu
    // We therefore find that f(x, u) = Ax + Bu + [[0] [m * g * r / I *
    // cos(theta)]]
    Matrix<N2, N1> updatedXhat =
        NumericalIntegration.rkf45(
            (Matrix<N2, N1> x, Matrix<N1, N1> u_) -> {
              Matrix<N2, N1> xdot = m_plant.getA().times(x).plus(m_plant.getB().times(u_));
              if (m_simulateGravity) {
                xdot =
                    xdot.plus(
                        VecBuilder.fill(
                            0,
                            m_armMass
                                * m_r
                                * -9.8
                                * 3.0
                                / (m_armMass * m_r * m_r)
                                * Math.cos(x.get(0, 0))));
              }
              return xdot;
            },
            currentXhat,
            u,
            dtSeconds);

    // We check for collision after updating xhat
    if (wouldHitLowerLimit(updatedXhat.get(0, 0))) {
      return VecBuilder.fill(m_minAngle, 0);
    }
    if (wouldHitUpperLimit(updatedXhat.get(0, 0))) {
      return VecBuilder.fill(m_maxAngle, 0);
    }
    return updatedXhat;
  }
}
