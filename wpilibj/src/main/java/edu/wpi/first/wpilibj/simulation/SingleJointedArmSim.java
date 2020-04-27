/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.wpilibj.system.LinearSystem;
import edu.wpi.first.wpilibj.system.RungeKutta;
import edu.wpi.first.wpilibj.system.plant.DCMotor;
import edu.wpi.first.wpilibj.system.plant.LinearSystemId;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.VecBuilder;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N2;

public class SingleJointedArmSim extends LinearSystemSim<N2, N1, N1> {

  @SuppressWarnings("MemberName")
  private final double m_r;
  private double m_maxAngleRads;
  private double m_minAngleRads;
  private final double m_armMass;

  /**
   * Create a LinearSystemSimulator. This simulator uses an {@link LinearSystem} to simulate
   * the state of the system. In simulationPeriodic, users should first set inputs from motors, update
   * the simulation and write simulated outputs to sensors.
   *
   * @param armSystem            The arm system being controlled.
   * @param m_measurementStdDevs Standard deviations of measurements. Can be null if addNoise is false.
   * @param armMassKg            The mass of the arm.
   * @param armLengthMeters      The distance from the pivot of the arm to its center of mass.
   *                             This number is not the same as the overall length of the arm.
   */
  public SingleJointedArmSim(LinearSystem<N2, N1, N1> armSystem, double armMassKg,
                             double armLengthMeters, double minAngleRads, double maxAngleRads,
                             Matrix<N1, N1> m_measurementStdDevs) {
    super(armSystem, m_measurementStdDevs);
    this.m_armMass = armMassKg;
    this.m_r = armLengthMeters;
    this.m_minAngleRads = minAngleRads;
    this.m_maxAngleRads = maxAngleRads;
  }

  /**
   * Create a LinearSystemSimulator. This simulator uses an {@link LinearSystem} to simulate
   * the state of the system. In simulationPeriodic, users should first set inputs from motors, update
   * the simulation and write simulated outputs to sensors.
   *  @param armMassKg            The mass of the arm.
   * @param armLengthMeters      The distance from the pivot of the arm to its center of mass.
   * @param m_measurementStdDevs Standard deviations of measurements. Can be null if addNoise is false.
   */
  public SingleJointedArmSim(DCMotor motor, double jKgSquaredMeters,
                             double G, double armMassKg, double armLengthMeters,
                             double minAngleRads, double maxAngleRads,
                             Matrix<N1, N1> m_measurementStdDevs) {
    this(LinearSystemId.createSingleJointedArmSystem(motor, jKgSquaredMeters, G),
        armMassKg, armLengthMeters, minAngleRads, maxAngleRads,
        m_measurementStdDevs);
  }

  /**
   * Create a LinearSystemSimulator. This simulator uses an {@link LinearSystem} to simulate
   * the state of the system. In simulationPeriodic, users should first set inputs from motors, update
   * the simulation and write simulated outputs to sensors.
   *  @param armMassKg            The mass of the arm.
   * @param armLengthMeters      The distance from the pivot of the arm to its center of mass.
   * @param m_measurementStdDevs Standard deviations of measurements. Can be null if addNoise is false.
   */
  public SingleJointedArmSim(DCMotor motor,
                             double G, double armMassKg, double armLengthMeters,
                             double minAngleRads, double maxAngleRads,
                             Matrix<N1, N1> m_measurementStdDevs) {
    this(LinearSystemId.createSingleJointedArmSystem(motor,
        1.0 / 3.0 * armMassKg * armLengthMeters * armLengthMeters, G),
        armMassKg, armLengthMeters, minAngleRads, maxAngleRads,
        m_measurementStdDevs);
  }

  public boolean hasHitLowerLimit(Matrix<N2, N1> x) {
    return x.get(0, 0) < this.m_minAngleRads;
  }

  public boolean hasHitUpperLimit(Matrix<N2, N1> x) {
    return x.get(0, 0) > this.m_maxAngleRads;
  }

  @Override
  protected Matrix<N2, N1> updateX(Matrix<N2, N1> currentXhat, Matrix<N1, N1> u, double dtSeconds) {
    /*
    Horizontal case:
    Torque = f * r = I * alpha
    alpha = f * r / I
    since f=mg,
    alpha = m g r / I

    Multiply RHS by cos(theta) to account for the arm angle
     */
    Matrix<N2, N1> updatedXhat = RungeKutta.rungeKutta(
        (x, u_) -> (m_plant.getA().times(x))
            .plus(m_plant.getB().times(u_))
            .plus(VecBuilder.fill(0,
                m_armMass * m_r * -9.8 * 3.0 / (m_armMass * m_r * m_r) * Math.cos(x.get(0, 0)))),
        currentXhat, u, dtSeconds);

    // We check for collision after updating xhat
    if (hasHitLowerLimit(updatedXhat)) {
      return VecBuilder.fill(m_minAngleRads, 0);
    } else if (hasHitUpperLimit(updatedXhat)) {
      return VecBuilder.fill(m_maxAngleRads, 0);
    }

    return updatedXhat;
  }

  public double getArmAngleRads() {
    return m_x.get(0, 0);
  }

  public double getArmVelocityRadPerSec() {
    return m_x.get(1, 0);
  }
}
