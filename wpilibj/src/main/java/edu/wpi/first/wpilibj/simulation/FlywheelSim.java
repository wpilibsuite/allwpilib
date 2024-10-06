// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static edu.wpi.first.units.Units.Amps;
import static edu.wpi.first.units.Units.KilogramSquareMeters;
import static edu.wpi.first.units.Units.Volts;

import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.math.system.plant.Gearbox;
import edu.wpi.first.math.system.plant.LinearSystemId;
import edu.wpi.first.units.measure.Voltage;
import edu.wpi.first.wpilibj.RobotController;

/** Represents a simulated flywheel mechanism controlled by voltage input. */
public class FlywheelSim extends FlywheelSimBase {
  /**
   * Creates a simulated flywheel mechanism controlled by voltage input.
   *
   * @param plant The linear system that represents the flywheel controlled by voltage input. Use
   *     either {@link LinearSystemId#createFlywheelSystem(DCMotor, double, double)} if using
   *     physical constants or {@link LinearSystemId#identifyVelocitySystem(double, double)} if
   *     using system characterization.
   * @param gearbox The type of and number of motors in the flywheel gearbox.
   * @param measurementStdDevs The standard deviations of the measurements. Can be omitted if no
   *     noise is desired. If present must have 1 element for velocity.
   */
  public FlywheelSim(
      LinearSystem<N1, N1, N1> plant, Gearbox gearbox, double... measurementStdDevs) {
    // By theorem 6.10.1 of
    // https://file.tavsys.net/control/controls-engineering-in-frc.pdf,
    // the flywheel state-space model with voltage as input is:
    //
    // dx/dt = -nG²Kₜ/(KᵥRJ)x + (nGKₜ)/(RJ)u
    // A = -nG²Kₜ/(KᵥRJ)
    // B = nGKₜ/(RJ)
    //
    // Solve for J.
    //
    // B = nGKₜ/(RJ)
    // J = nGKₜ/(RB)
    super(
        plant,
        gearbox,
        KilogramSquareMeters.of(
            gearbox.numMotors
                * gearbox.reduction
                * gearbox.motorType.KtNMPerAmp
                / gearbox.motorType.rOhms
                / plant.getB(0, 0)),
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
    m_voltage.mut_replace(m_u.get(0, 0), Volts);
  }

  /**
   * Sets the input voltage for the flywheel.
   *
   * @param voltage The input voltage.
   */
  public void setInputVoltage(Voltage voltage) {
    setInputVoltage(voltage.in(Volts));
  }

  @Override
  public void update(double dtSeconds) {
    super.update(dtSeconds);
    m_currentDraw.mut_replace(
        m_gearbox.currentAmps(m_x.get(0, 0), getInput(0)) * Math.signum(m_u.get(0, 0)), Amps);
    m_voltage.mut_replace(getInput(0), Volts);
    m_torque.mut_replace(m_gearbox.torque(m_currentDraw));
  }
}
