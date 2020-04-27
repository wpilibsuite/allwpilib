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

public class ElevatorSim extends LinearSystemSim<N2, N1, N1> {

  private final DCMotor m_motor;
  private final double m_drumRadiusMeters;
  private double m_minHeightMeters;
  private double m_maxHeightMeters;
  private final double m_gearing;

  /**
   * Create a LinearSystemSimulator. This simulator uses an {@link LinearSystem} to simulate
   * * the state of the system. In simulationPeriodic, users should first set inputs from motors, update
   * * the simulation and write simulated outputs to sensors.
   *
   * @param elevatorGearbox      The {@link DCMotor} representing the elevator motor.
   * @param carriageMassKg       The mass of the carriage.
   * @param drumRadiusMeters     The radius of the drum driving the elevator.
   * @param gearingReduction     The reduction between motor and drum, as output over input.
   *                             In most cases this is greater than one.
   * @param m_measurementStdDevs Standard deviations for position noise. Can be null
   *                             if no added noise is desired.
   */
  public ElevatorSim(DCMotor elevatorGearbox, double carriageMassKg,
                     double drumRadiusMeters, double gearingReduction,
                     double minHeightMeters, double maxHeightMeters,
                     Matrix<N1, N1> m_measurementStdDevs) {
    super(LinearSystemId.createElevatorSystem(elevatorGearbox, carriageMassKg, drumRadiusMeters,
        gearingReduction), m_measurementStdDevs);
    this.m_motor = elevatorGearbox;
    this.m_gearing = gearingReduction;
    this.m_drumRadiusMeters = drumRadiusMeters;
    this.m_minHeightMeters = minHeightMeters;
    this.m_maxHeightMeters = maxHeightMeters;
  }

  /**
   * Create a LinearSystemSimulator. This simulator uses an {@link LinearSystem} to simulate
   * the state of the system. In simulationPeriodic, users should first set inputs from motors, update
   * the simulation and write simulated outputs to sensors.
   *
   * @param elevatorPlant        The elevator system being controlled. This system can be created
   *                             with {@link edu.wpi.first.wpilibj.system.plant.LinearSystemId#createElevatorSystem(DCMotor, double, double, double)}
   *                             or {@link edu.wpi.first.wpilibj.system.plant.LinearSystemId#identifyPositionSystem(double, double)}.
   * @param m_measurementStdDevs Standard deviations of measurements. Can be null if addNoise is false.
   */
  public ElevatorSim(LinearSystem<N2, N1, N1> elevatorPlant,
                     Matrix<N1, N1> m_measurementStdDevs, DCMotor elevatorGearbox,
                     double gearingReduction, double drumRadiusMeters,
                     double minHeightMeters, double maxHeightMeters) {
    super(elevatorPlant, m_measurementStdDevs);
    this.m_motor = elevatorGearbox;
    this.m_gearing = gearingReduction;
    this.m_drumRadiusMeters = drumRadiusMeters;
    this.m_minHeightMeters = minHeightMeters;
    this.m_maxHeightMeters = maxHeightMeters;
  }

  public boolean hasHitLowerLimit(Matrix<N2, N1> x) {
    return x.get(0, 0) < this.m_minHeightMeters;
  }

  public boolean hasHitUpperLimit(Matrix<N2, N1> x) {
    return x.get(0, 0) > this.m_maxHeightMeters;
  }

  @Override
  protected Matrix<N2, N1> updateX(Matrix<N2, N1> currentXhat, Matrix<N1, N1> u, double dtSeconds) {
    var updatedXhat = RungeKutta.rungeKutta((x, u_) -> (m_plant.getA().times(x)).plus(m_plant.getB().times(u_)).plus(VecBuilder.fill(0, -9.8)),
        currentXhat, u, dtSeconds);

    // We check for collision after updating xhat
    if (hasHitLowerLimit(updatedXhat)) {
      return VecBuilder.fill(m_minHeightMeters, 0);
    } else if (hasHitUpperLimit(updatedXhat)) {
      return VecBuilder.fill(m_maxHeightMeters, 0);
    }

    return updatedXhat;
  }

  public double getElevatorPositionMeters() {
    return getOutput(0);
  }

  public double getElevatorVelocityMetersPerSecond() {
    return m_x.get(0, 1);
  }

  @Override
  public double getCurrentDrawAmps() {
    // I = V / R - omega / (Kv * R)
    // Reductions are greater than 1, so a reduction of 10:1 would mean the motor is
    // spinning 10x faster than the output
    // v = r w, so w = v/r
    double velocityMps = m_x.get(1, 0);
    double motorVelocityRadPerSec = velocityMps / m_drumRadiusMeters * m_gearing;
    return m_u.get(0, 0) / m_motor.m_rOhms
          - motorVelocityRadPerSec / (m_motor.m_KvRadPerSecPerVolt * m_motor.m_rOhms);
  }
}
