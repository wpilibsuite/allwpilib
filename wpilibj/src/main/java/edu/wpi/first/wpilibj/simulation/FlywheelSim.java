/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.wpilibj.system.LinearSystem;
import edu.wpi.first.wpilibj.system.plant.DCMotor;
import edu.wpi.first.wpilibj.system.plant.LinearSystemId;
import edu.wpi.first.wpilibj.util.Units;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.numbers.N1;

public class FlywheelSim extends LinearSystemSim<N1, N1, N1> {

  private final DCMotor m_motor;
  private final double m_gearing;

  /**
   * Create a LinearSystemSimulator. This simulator uses an {@link LinearSystem} to simulate
   * the state of the system. In simulationPeriodic, users should first set inputs from motors, update
   * the simulation and write simulated outputs to sensors.
   *
   * @param flywheelPlant      The flywheel system being controlled. This system can be created
   *                           using {@link LinearSystemId#createFlywheelSystem(DCMotor, double, double)}
   *                           or {@link LinearSystemId#identifyVelocitySystem(double, double)}
   * @param flywheelGearbox    The DCMotor representing the motor driving the flywheel.
   * @param gearing            The reduction between motor and drum, as output over input.
   *                           In most cases this is greater than one.
   * @param measurementStdDevs Standard deviations of measurements. Can be null if addNoise is false.
   */
  public FlywheelSim(LinearSystem<N1, N1, N1> flywheelPlant, DCMotor flywheelGearbox,
                     double gearing, Matrix<N1, N1> measurementStdDevs) {
    super(flywheelPlant, measurementStdDevs);
    this.m_motor = flywheelGearbox;
    this.m_gearing = gearing;
  }

  /**
   * Create a LinearSystemSimulator. This simulator uses an {@link LinearSystem} to simulate
   * the state of the system. In simulationPeriodic, users should first set inputs from motors, update
   * the simulation and write simulated outputs to sensors.
   *
   * @param flywheelGearbox    The DCMotor representing the motor driving the flywheel.
   * @param gearing            The reduction between motor and drum, as output over input.
   *                           In most cases this is greater than one.
   * @param moiKgMetersSquared The moment of inertia J of the flywheel. This can be
   *                           calculated with CAD. If J is unknown, {@link #FlywheelSim(LinearSystem, DCMotor, double, Matrix)}
   *                           using FRC-Characterization's kV and kA may be better.
   * @param measurementStdDevs Standard deviations of measurements. Can be null if addNoise is false.
   */
  public FlywheelSim(DCMotor flywheelGearbox, double gearing,
                     double moiKgMetersSquared, Matrix<N1, N1> measurementStdDevs) {
    super(LinearSystemId.createFlywheelSystem(flywheelGearbox, moiKgMetersSquared, gearing),
        measurementStdDevs);
    this.m_motor = flywheelGearbox;
    this.m_gearing = gearing;
  }

  public double getAngularVelocityRadPerSec() {
    return getOutput(0);
  }

  public double getAngularVelocityRPM() {
    return Units.radiansPerSecondToRotationsPerMinute(getOutput(0));
  }

  @Override
  public double getCurrentDrawAmps() {
    // I = V / R - omega / (Kv * R)
    // Reductions are output over input, so a reduction of 2:1 means the motor is spinning
    // 2x faster than the flywheel
    return m_motor.getCurrent(getAngularVelocityRadPerSec() * m_gearing, m_u.get(0, 0))
        * Math.signum(m_u.get(0, 0));
  }
}
