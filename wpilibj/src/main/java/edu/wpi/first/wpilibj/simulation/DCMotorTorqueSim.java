// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.plant.DCMotor;

/** Represents a simulated DC motor mechanism. */
public class DCMotorTorqueSim extends DCMotorSim {

  /**
   * Creates a simulated DC motor mechanism.
   *
   * @param plant The linear system representing the DC motor. This system can be created with
   *     {@link edu.wpi.first.math.system.plant.LinearSystemId#createDCMotorSystem(DCMotor, double,
   *     double)} or {@link
   *     edu.wpi.first.math.system.plant.LinearSystemId#createDCMotorSystem(double, double)}. If
   *     {@link edu.wpi.first.math.system.plant.LinearSystemId#createDCMotorSystem(double, double)}
   *     is used, the distance unit must be radians.
   * @param gearbox The type of and number of motors in the DC motor gearbox.
   * @param measurementStdDevs The standard deviations of the measurements. Can be omitted if no
   *     noise is desired. If present must have 2 elements. The first element is for position. The
   *     second element is for velocity.
   */
  public DCMotorTorqueSim(LinearSystem<N2, N1, N2> plant, DCMotor gearbox, double... measurementStdDevs) {
    super(plant, gearbox, measurementStdDevs);

  }

  /**
   * Sets the input torque for the flywheel.
   *
   * @param volts The input torque.
   */
  public void setInputTorque(double torqueNM) {
    setInput(torqueNM);
    // TODO:  Need some guidance on clamping.
  }

  @Override
  public double getTorqueNewtonMeters() {
    // I = V / R - omega / (Kv * R)
    // Reductions are output over input, so a reduction of 2:1 means the motor is
    // spinning
    // 2x faster than the flywheel
    return getInput(0);
  }

  @Override
  public double getVoltage() {
    // I = V / R - omega / (Kv * R)
    // torque = J * omega
    // V = R * torque / J + omega / Kv
    // Reductions are output over input, so a reduction of 2:1 means the motor is
    // spinning
    // 2x faster than the flywheel
    return m_gearbox.getVoltage(
        m_u.get(0, 0),
        m_x.get(1, 0) * m_gearing) * Math.signum(m_u.get(0, 0));
  }
}