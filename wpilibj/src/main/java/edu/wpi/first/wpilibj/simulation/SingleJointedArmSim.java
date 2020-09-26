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
  private final double m_maxAngleRads;
  private final double m_minAngleRads;
  private final double m_armMass;
  private final DCMotor m_motor;
  private final double m_gearing;
  private final boolean m_simulateGravity;

  /**
   * Create a LinearSystemSimulator. This simulator uses an {@link LinearSystem} to simulate
   * the state of the system. In simulationPeriodic, users should first set inputs from motors, update
   * the simulation and write simulated outputs to sensors.
   *
   * @param armSystem            The arm system being controlled.
   * @param motor                DCMotor representing the motor driving the arm.
   * @param G                    The gear ratio of the arm (numbers greater than 1
   *                             represent reductions).
   * @param armMassKg            The mass of the arm.
   * @param armLengthMeters      The distance from the pivot of the arm to its center of mass.
   *                             This number is not the same as the overall length of the arm.
   * @param minAngleRads         The min angle the arm can reach, with 0 being measured from
   *                             horizontal. The simulation will not allow motion past this.
   * @param maxAngleRads         The max angle the arm can reach, with 0 being measured from
   *                             horizontal. The simulation will not allow motion past this.
   * @param posMeasurementStdDev Standard deviations of noise in the arm's position measurement.
   * @param simulateGravity      If the affects of gravity should be simulated.
   */
  public SingleJointedArmSim(LinearSystem<N2, N1, N1> armSystem,
                             DCMotor motor, double G,
                             double armMassKg,
                             double armLengthMeters, double minAngleRads, double maxAngleRads,
                             double posMeasurementStdDev, boolean simulateGravity) {
    super(armSystem, VecBuilder.fill(posMeasurementStdDev));
    this.m_armMass = armMassKg;
    this.m_r = armLengthMeters;
    this.m_minAngleRads = minAngleRads;
    this.m_maxAngleRads = maxAngleRads;
    this.m_motor = motor;
    this.m_gearing = G;
    this.m_simulateGravity = simulateGravity;
  }

  /**
   * Create a LinearSystemSimulator. This simulator uses an {@link LinearSystem} to simulate
   * the state of the system. In simulationPeriodic, users should first set inputs from motors, update
   * the simulation and write simulated outputs to sensors.
   *
   * @param motor                DCMotor representing the motor driving the arm.
   * @param jKgSquaredMeters     The moment of inertia of the arm.
   * @param G                    The gear ratio of the arm (numbers greater than 1
   *                             represent reductions).
   * @param armMassKg            The mass of the arm.
   * @param armLengthMeters      The distance from the pivot of the arm to its center of mass.
   * @param minAngleRads         The min angle the arm can reach, with 0 being measured from
   *                             horizontal. The simulation will not allow motion past this.
   * @param maxAngleRads         The max angle the arm can reach, with 0 being measured from
   *                             horizontal. The simulation will not allow motion past this.
   * @param posMeasurementStdDev Standard deviations of noise in the arm's position measurement.
   * @param simulateGravity      If the affects of gravity should be simulated.
   */
  public SingleJointedArmSim(DCMotor motor, double jKgSquaredMeters,
                             double G, double armMassKg, double armLengthMeters,
                             double minAngleRads, double maxAngleRads,
                             double posMeasurementStdDev, boolean simulateGravity) {
    this(LinearSystemId.createSingleJointedArmSystem(motor, jKgSquaredMeters, G),
        motor, G,
        armMassKg, armLengthMeters, minAngleRads, maxAngleRads,
        posMeasurementStdDev, simulateGravity);
  }

  /**
   * Create a LinearSystemSimulator. This simulator uses an {@link LinearSystem} to simulate
   * the state of the system. In simulationPeriodic, users should first set inputs from motors, update
   * the simulation and write simulated outputs to sensors.
   *
   * @param motor                      DCMotor representing the motor driving the arm.
   * @param G                          The gear ratio of the arm (numbers greater than 1
   *                                   represent reductions).
   * @param armMassKg                  The mass of the arm.
   * @param armLengthMeters            The distance from the pivot of the arm to its center of mass.
   * @param minAngleRads               The min angle the arm can reach, with 0 being measured from
   *                                   horizontal. The simulation will not allow motion past this.
   * @param maxAngleRads               The max angle the arm can reach, with 0 being measured from
   *                                   horizontal. The simulation will not allow motion past this.
   * @param positionMeasurementStdDevs Standard deviations of noise in the arm's position measurement.
   * @param simulateGravity      If the affects of gravity should be simulated.
   */
  public SingleJointedArmSim(DCMotor motor,
                             double G, double armMassKg, double armLengthMeters,
                             double minAngleRads, double maxAngleRads,
                             double positionMeasurementStdDevs, boolean simulateGravity) {
    this(LinearSystemId.createSingleJointedArmSystem(motor,
        1.0 / 3.0 * armMassKg * armLengthMeters * armLengthMeters, G),
        motor, G,
        armMassKg, armLengthMeters, minAngleRads, maxAngleRads,
        positionMeasurementStdDevs, simulateGravity);
  }


  /**
   * Create a LinearSystemSimulator. This simulator uses an {@link LinearSystem} to simulate
   * the state of the system. In simulationPeriodic, users should first set inputs from motors, update
   * the simulation and write simulated outputs to sensors.
   *
   * @param motor                      DCMotor representing the motor driving the arm.
   * @param G                          The gear ratio of the arm (numbers greater than 1
   *                                   represent reductions).
   * @param armMassKg                  The mass of the arm.
   * @param armLengthMeters            The distance from the pivot of the arm to its center of mass.
   * @param minAngleRads               The min angle the arm can reach, with 0 being measured from
   *                                   horizontal. The simulation will not allow motion past this.
   * @param maxAngleRads               The max angle the arm can reach, with 0 being measured from
   *                                   horizontal. The simulation will not allow motion past this.
   * @param positionMeasurementStdDevs Standard deviations of noise in the arm's position measurement.
   */
  public SingleJointedArmSim(DCMotor motor,
                             double G, double armMassKg, double armLengthMeters,
                             double minAngleRads, double maxAngleRads,
                             double positionMeasurementStdDevs) {
    this(LinearSystemId.createSingleJointedArmSystem(motor,
        1.0 / 3.0 * armMassKg * armLengthMeters * armLengthMeters, G),
        motor, G,
        armMassKg, armLengthMeters, minAngleRads, maxAngleRads,
        positionMeasurementStdDevs, true);
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

    This acceleration is added to the linear system dynamics x-dot = Ax + Bu
    We therefore find that f(x, u) = Ax + Bu + [[0] [m * g * r / I]]
     */
    Matrix<N2, N1> updatedXhat = RungeKutta.rungeKutta(
        this::dynamics,
        currentXhat, u, dtSeconds);

    // We check for collision after updating xhat
    if (hasHitLowerLimit(updatedXhat)) {
      return VecBuilder.fill(m_minAngleRads, 0);
    } else if (hasHitUpperLimit(updatedXhat)) {
      return VecBuilder.fill(m_maxAngleRads, 0);
    }

    return updatedXhat;
  }

  public double getAngleRads() {
    return m_x.get(0, 0);
  }

  public double getVelocityRadPerSec() {
    return m_x.get(1, 0);
  }

  @Override
  public double getCurrentDrawAmps() {
    // Reductions are greater than 1, so a reduction of 10:1 would mean the motor is
    // spinning 10x faster than the output
    var motorVelocity = m_x.get(1, 0) * m_gearing;
    return m_motor.getCurrent(motorVelocity, m_u.get(0, 0)) * Math.signum(m_u.get(0, 0));
  }

  private Matrix<N2, N1> dynamics(Matrix<N2, N1> x, Matrix<N1, N1> u_) {
    Matrix<N2, N1> xdot = (m_plant.getA().times(x))
        .plus(m_plant.getB().times(u_));

    if (m_simulateGravity) {
      xdot = xdot.plus(VecBuilder.fill(0,
              m_armMass * m_r * -9.8 * 3.0 / (m_armMass * m_r * m_r) * Math.cos(x.get(0, 0))));
    }
    return xdot;
  }
}
