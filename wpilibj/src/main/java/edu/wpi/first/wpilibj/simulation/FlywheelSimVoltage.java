// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.math.system.plant.LinearSystemId;
import edu.wpi.first.wpilibj.RobotController;

/** Represents a simulated flywheel mechanism controlled by voltage input. */
public class FlywheelSimVoltage extends FlywheelSim {
  /**
   * Creates a simulated flywheel mechanism controlled by voltage input.
   *
   * @param plant              The linear system that represents the flywheel
   *                           controlled by voltage input. Use
   *                           either {@link
   *                           LinearSystemId#createFlywheelSystem(DCMotor, double, double)}
   *                           if using physical constants
   *                           or
   *                           {@link LinearSystemId#identifyVelocitySystem(double, double)}
   *                           if using system
   *                           characterization.
   * @param gearbox            The type of and number of motors in the flywheel
   *                           gearbox.
   * @param measurementStdDevs The standard deviations of the measurements. Can be
   *                           omitted if no
   *                           noise is desired. If present must have 1 element
   *                           for velocity.
   */
  public FlywheelSimVoltage(
      LinearSystem<N1, N1, N1> plant, DCMotor gearbox, double... measurementStdDevs) {
    // By theorem 6.10.1 of
    // https://file.tavsys.net/control/controls-engineering-in-frc.pdf,
    // the flywheel state-space model with voltage as input is:
    //
    // dx/dt = -G²Kₜ/(KᵥRJ)x + (GKₜ)/(RJ)u
    // A = -G²Kₜ/(KᵥRJ)
    // B = GKₜ/(RJ)
    //
    // Solve for G.
    //
    // A/B = -G/Kᵥ
    // G = -KᵥA/B
    //
    // Solve for J.
    //
    // B = GKₜ/(RJ)
    // J = GKₜ/(RB)
    // J = -KᵥKₜA/(RB²)
    super(
        plant,
        gearbox,
        -gearbox.KvRadPerSecPerVolt * plant.getA(0, 0) / plant.getB(0, 0),
        -gearbox.KvRadPerSecPerVolt * -gearbox.KtNMPerAmp * plant.getA(0, 0) / gearbox.rOhms
            * Math.pow(plant.getB(0, 0), 2),
        measurementStdDevs);
  }

  /**
   * Sets the input voltage for the flywheel.
   *
   * @param volts The input voltage.
   */
  public void setInputVoltage(double volts) {
    setInput(volts);
    clampInput(RobotController.getBatteryVoltage());
  }

  /**
   * Gets the torque on the flywheel.
   *
   * @return The flywheel's torque in Newton-Meters.
   */
  @Override
  public double getTorqueNewtonMeters() {
    return getAngularAccelerationRadPerSecSq() * m_jKgMetersSquared;
  }

  /**
   * Gets the voltage of the flywheel.
   *
   * @return The flywheel's voltage.
   */
  @Override
  public double getVoltage() {
    return getInput(0);
  }
}
