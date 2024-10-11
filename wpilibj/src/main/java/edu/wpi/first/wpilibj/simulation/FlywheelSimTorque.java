// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static edu.wpi.first.units.Units.Amps;
import static edu.wpi.first.units.Units.KilogramSquareMeters;
import static edu.wpi.first.units.Units.NewtonMeters;
import static edu.wpi.first.units.Units.Volts;

import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.plant.Gearbox;
import edu.wpi.first.math.system.plant.LinearSystemId;
import edu.wpi.first.units.measure.MomentOfInertia;
import edu.wpi.first.units.measure.Torque;

/** Represents a simulated flywheel mechanism controlled by torque input. */
public class FlywheelSimTorque extends FlywheelSimBase {
  /**
   * Creates a simulated flywheel mechanism controlled by torque input.
   *
   * <p>If using physical constants create the plant using either {@link
   * LinearSystemId#createFlywheelSystem(double)} or {@link
   * LinearSystemId#createFlywheelSystem(MomentOfInertia)}.
   *
   * <p>If using system characterization create the plant using either {@link
   * LinearSystemId#identifyVelocitySystem(double, double)} or the units class overload.
   *
   * @param plant The linear system that represents the simulated flywheel mechanism controlled by
   *     torque input.
   * @param gearbox The gearbox of the simulated flywheel mechanism.
   * @param measurementStdDevs The standard deviations of the measurements. Can be omitted if no
   *     noise is desired. If present must have 1 element for velocity. The units should be radians
   *     per second.
   */
  public FlywheelSimTorque(
      LinearSystem<N1, N1, N1> plant, Gearbox gearbox, double... measurementStdDevs) {
    // By equations 12.17 of
    // https://file.tavsys.net/control/controls-engineering-in-frc.pdf,
    // the torque applied to the flywheel is τ = Jα.
    // the flywheel state-space model with torque as input is:
    //
    // dx/dt = 0 x + 1/J u
    // A = 0
    // B = 1/J
    //
    // Solve for J.
    //
    // B = 1/J
    // J = 1/B
    super(
        plant, gearbox, KilogramSquareMeters.of(1.0 / plant.getB().get(0, 0)), measurementStdDevs);
  }

  /**
   * Sets the input torque of the simulated flywheel mechanism.
   *
   * @param torqueNewtonMeters The input torque in newton-meters.
   */
  public void setInputTorque(double torqueNewtonMeters) {
    setInput(torqueNewtonMeters);
    // TODO: Need some guidance on clamping.
    m_torque.mut_replace(m_u.get(0, 0), NewtonMeters);
  }

  /**
   * Sets the input torque of the simulated flywheel mechanism.
   *
   * @param torque The input torque.
   */
  public void setInputTorque(Torque torque) {
    setInputTorque(torque.in(NewtonMeters));
  }

  @Override
  public void update(double dtSeconds) {
    super.update(dtSeconds);
    m_currentDraw.mut_replace(
        m_gearbox.currentAmps(getInput(0)) * Math.signum(m_u.get(0, 0)), Amps);
    m_voltage.mut_replace(m_gearbox.voltage(getInput(0), m_x.get(0, 0)), Volts);
    m_torque.mut_replace(getInput(0), NewtonMeters);
  }
}
