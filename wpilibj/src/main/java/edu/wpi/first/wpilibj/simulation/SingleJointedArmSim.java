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

/** Represents a simulated single jointed arm mechanism. */
public class SingleJointedArmSim extends AngularMechanismSim {
  // The length of the arm.
  private final double m_armLenMeters;

  // The pivot point of the arm.
  private final double m_pivotPointMeters;

  // The minimum angle that the arm is capable of.
  private final double m_minAngle;

  // The maximum angle that the arm is capable of.
  private final double m_maxAngle;

  // Whether the simulator should simulate gravity.
  private final boolean m_simulateGravity;

  /**
   * Creates a simulated arm mechanism.
   *
   * @param plant The linear system that represents the arm. This system can be created with {@link
   *     LinearSystemId#createSingleJointedArmSystem(DCMotor, double, double, double, double)} or
   *     {@link LinearSystemId#identifyPositionSystem(double, double)}.
   * @param gearbox The type of and number of motors in the arm gearbox.
   * @param armLengthMeters The length of the arm.
   * @param pivotPointMeters The pivot point of the arm.
   * @param minAngleRads The minimum angle that the arm is capable of.
   * @param maxAngleRads The maximum angle that the arm is capable of.
   * @param simulateGravity Whether gravity should be simulated or not.
   * @param startingAngleRads The initial position of the Arm simulation in radians.
   * @param measurementStdDevs The standard deviations of the measurements. Can be omitted if no
   *     noise is desired. If present must have 1 element for position.
   */
  @SuppressWarnings("this-escape")
  public SingleJointedArmSim(
      LinearSystem<N2, N1, N2> plant,
      DCMotor gearbox,
      double armLengthMeters,
      double pivotPointMeters,
      double minAngleRads,
      double maxAngleRads,
      boolean simulateGravity,
      double startingAngleRads,
      double... measurementStdDevs) {
    super(plant, gearbox, measurementStdDevs);
    m_armLenMeters = armLengthMeters;
    m_pivotPointMeters = pivotPointMeters;
    m_minAngle = minAngleRads;
    m_maxAngle = maxAngleRads;
    m_simulateGravity = simulateGravity;

    setState(startingAngleRads, 0.0);
  }

  /**
   * Sets the arm's state. The new angle will be limited between the minimum and maximum allowed
   * limits.
   *
   * @param angleRadians The new angle in radians.
   * @param velocityRadPerSec The new angular velocity in radians per second.
   */
  @Override
  public final void setState(double angleRadians, double velocityRadPerSec) {
    setState(
        VecBuilder.fill(MathUtil.clamp(angleRadians, m_minAngle, m_maxAngle), velocityRadPerSec));
  }

  @Override
  public void setPosition(double angularPositionRad) {
    super.setPosition(MathUtil.clamp(angularPositionRad, m_minAngle, m_maxAngle));
  }

  @Override
  public double getAngularAccelerationRadPerSecSq() {
    // The torque on the arm is given by τ = F⋅r, where F is the force applied by
    // gravity and r the distance from pivot to center of mass. Recall from
    // dynamics that the sum of torques for a rigid body is τ = J⋅α, were τ is
    // torque on the arm, J is the mass-moment of inertia about the pivot axis,
    // and α is the angular acceleration in rad/s². Rearranging yields: α = F⋅r/J
    //
    // We substitute in F = m⋅g⋅cos(θ), where θ is the angle from horizontal:
    //
    //   α = (m⋅g⋅cos(θ))⋅r/J
    //
    // This acceleration is next added to the linear system dynamics ẋ=Ax+Bu
    //
    //   f(x, u) = Ax + Bu + [0  α]ᵀ
    //   f(x, u) = Ax + Bu + [0  3/2⋅g⋅cos(θ)/L]ᵀ
    double a = super.getAngularAccelerationRadPerSecSq();
    if (m_simulateGravity) {
      double m = getMassKilograms();
      double g = -9.8;
      double l = getArmLengthMeters();
      double p = getPivotPointMeters();
      double r = Math.abs((l / 2) - p);
      double J = getJKgMetersSquared();

      double alphaGrav = (m * g * r / J) * Math.cos(m_x.get(0, 0));
      a += alphaGrav;
    }
    return a;
  }

  /**
   * Returns the arm length of the system.
   *
   * @return the arm length.
   */
  public double getArmLengthMeters() {
    return m_armLenMeters;
  }

  /**
   * Returns the pivot point of the system.
   *
   * @return the pivot point.
   */
  public double getPivotPointMeters() {
    return m_pivotPointMeters;
  }

  /**
   * Returns the minimum angle of the system.
   *
   * @return the minimum angle.
   */
  public double getMinimumAngleRadians() {
    return m_minAngle;
  }

  /**
   * Returns the minimum angle of the system.
   *
   * @return the minimum angle.
   */
  public double getMaximumAngleRadians() {
    return m_maxAngle;
  }

  /**
   * Returns the mass of the system.
   *
   * @return the mass.
   */
  public double getMassKilograms() {
    double l = getArmLengthMeters();
    double p = getPivotPointMeters();
    double r = Math.abs((l / 2) - p);
    return getJKgMetersSquared() / ((1.0 / 12.0) * Math.pow(m_armLenMeters, 2) + Math.pow(r, 2));
  }

  /**
   * Returns whether the arm would hit the lower limit.
   *
   * @param currentAngleRads The current arm height.
   * @return Whether the arm would hit the lower limit.
   */
  public boolean wouldHitLowerLimit(double currentAngleRads) {
    return currentAngleRads <= this.m_minAngle;
  }

  /**
   * Returns whether the arm would hit the upper limit.
   *
   * @param currentAngleRads The current arm height.
   * @return Whether the arm would hit the upper limit.
   */
  public boolean wouldHitUpperLimit(double currentAngleRads) {
    return currentAngleRads >= this.m_maxAngle;
  }

  /**
   * Returns whether the arm has hit the lower limit.
   *
   * @return Whether the arm has hit the lower limit.
   */
  public boolean hasHitLowerLimit() {
    return wouldHitLowerLimit(getAngularPositionRad());
  }

  /**
   * Returns whether the arm has hit the upper limit.
   *
   * @return Whether the arm has hit the upper limit.
   */
  public boolean hasHitUpperLimit() {
    return wouldHitUpperLimit(getAngularPositionRad());
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
  protected Matrix<N2, N1> updateX(Matrix<N2, N1> currentXhat, Matrix<N1, N1> u, double dtSeconds) {
    // The torque on the arm is given by τ = F⋅r, where F is the force applied by
    // gravity and r the distance from pivot to center of mass. Recall from
    // dynamics that the sum of torques for a rigid body is τ = J⋅α, were τ is
    // torque on the arm, J is the mass-moment of inertia about the pivot axis,
    // and α is the angular acceleration in rad/s². Rearranging yields: α = F⋅r/J
    //
    // We substitute in F = m⋅g⋅cos(θ), where θ is the angle from horizontal:
    //
    //   α = (m⋅g⋅cos(θ))⋅r/J
    //
    // This acceleration is next added to the linear system dynamics ẋ=Ax+Bu
    //
    //   f(x, u) = Ax + Bu + [0  α]ᵀ
    //   f(x, u) = Ax + Bu + [0  3/2⋅g⋅cos(θ)/L]ᵀ

    Matrix<N2, N1> updatedXhat =
        NumericalIntegration.rkdp(
            (Matrix<N2, N1> x, Matrix<N1, N1> _u) -> {
              Matrix<N2, N1> xdot = m_plant.getA().times(x).plus(m_plant.getB().times(_u));
              if (m_simulateGravity) {
                double m = getMassKilograms();
                double g = -9.8;
                double l = getArmLengthMeters();
                double p = getPivotPointMeters();
                double r = Math.abs((l / 2) - p);
                double J = getJKgMetersSquared();

                double alphaGrav = (m * g * r / J) * Math.cos(x.get(0, 0));
                xdot = xdot.plus(VecBuilder.fill(0, alphaGrav));
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
