// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.math.system.plant.LinearSystemId;
import edu.wpi.first.math.util.Units;

/** Represents a simulated DC motor mechanism. */
public class DCMotorSim extends LinearSystemSim<N2, N1, N2> {
  // Gearbox for the DC motor.
  private final DCMotor m_gearbox;

  // The gearing from the motors to the output.
  private final double m_gearing;

  /**
   * Creates a simulated DC motor mechanism.
   *
   * @param plant The linear system that represents the DC motor.
   * @param gearbox The type of and number of motors in the DC motor gearbox.
   * @param gearing The gearing of the DC motor (numbers greater than 1 represent reductions).
   */
  public DCMotorSim(LinearSystem<N2, N1, N2> plant, DCMotor gearbox, double gearing) {
    super(plant);
    m_gearbox = gearbox;
    m_gearing = gearing;
  }

  /**
   * Creates a simulated DC motor mechanism.
   *
   * @param plant The linear system that represents the DC motor.
   * @param gearbox The type of and number of motors in the DC motor gearbox.
   * @param gearing The gearing of the DC motor (numbers greater than 1 represent reductions).
   * @param measurementStdDevs The standard deviations of the measurements.
   */
  public DCMotorSim(
      LinearSystem<N2, N1, N2> plant,
      DCMotor gearbox,
      double gearing,
      Matrix<N2, N1> measurementStdDevs) {
    super(plant, measurementStdDevs);
    m_gearbox = gearbox;
    m_gearing = gearing;
  }

  /**
   * Creates a simulated DC motor mechanism.
   *
   * @param gearbox The type of and number of motors in the DC motor gearbox.
   * @param gearing The gearing of the DC motor (numbers greater than 1 represent reductions).
   * @param jKgMetersSquared The moment of inertia of the DC motor. If this is unknown, use the
   *     {@link #DCMotorSim(LinearSystem, DCMotor, double, Matrix)} constructor.
   */
  public DCMotorSim(DCMotor gearbox, double gearing, double jKgMetersSquared) {
    super(LinearSystemId.createDCMotorSystem(gearbox, jKgMetersSquared, gearing));
    m_gearbox = gearbox;
    m_gearing = gearing;
  }

  /**
   * Creates a simulated DC motor mechanism.
   *
   * @param gearbox The type of and number of motors in the DC motor gearbox.
   * @param gearing The gearing of the DC motor (numbers greater than 1 represent reductions).
   * @param jKgMetersSquared The moment of inertia of the DC motor. If this is unknown, use the
   *     {@link #DCMotorSim(LinearSystem, DCMotor, double, Matrix)} constructor.
   * @param measurementStdDevs The standard deviations of the measurements.
   */
  public DCMotorSim(
      DCMotor gearbox, double gearing, double jKgMetersSquared, Matrix<N2, N1> measurementStdDevs) {
    super(
        LinearSystemId.createDCMotorSystem(gearbox, jKgMetersSquared, gearing), measurementStdDevs);
    m_gearbox = gearbox;
    m_gearing = gearing;
  }

  /**
   * Returns the DC motor position.
   *
   * @return The DC motor position.
   */
  public double getAngularPositionRad() {
    return getOutput(0);
  }

  /**
   * Returns the DC motor position in rotations.
   *
   * @return The DC motor position in rotations.
   */
  public double getAngularPositionRotations() {
    return Units.radiansToRotations(getAngularPositionRad());
  }

  /**
   * Returns the DC motor velocity.
   *
   * @return The DC motor velocity.
   */
  public double getAngularVelocityRadPerSec() {
    return getOutput(1);
  }

  /**
   * Returns the DC motor velocity in RPM.
   *
   * @return The DC motor velocity in RPM.
   */
  public double getAngularVelocityRPM() {
    return Units.radiansPerSecondToRotationsPerMinute(getAngularVelocityRadPerSec());
  }

  /**
   * Returns the DC motor current draw.
   *
   * @return The DC motor current draw.
   */
  @Override
  public double getCurrentDrawAmps() {
    // I = V / R - omega / (Kv * R)
    // Reductions are output over input, so a reduction of 2:1 means the motor is spinning
    // 2x faster than the output
    return m_gearbox.getCurrent(getAngularVelocityRadPerSec() * m_gearing, m_u.get(0, 0))
        * Math.signum(m_u.get(0, 0));
  }

  /**
   * Sets the input voltage for the DC motor.
   *
   * @param volts The input voltage.
   */
  public void setInputVoltage(double volts) {
    setInput(volts);
  }
}
