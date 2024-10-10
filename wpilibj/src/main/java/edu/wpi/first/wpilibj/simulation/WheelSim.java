// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static edu.wpi.first.units.Units.Amps;
import static edu.wpi.first.units.Units.Kilograms;
import static edu.wpi.first.units.Units.NewtonMeters;
import static edu.wpi.first.units.Units.Newtons;
import static edu.wpi.first.units.Units.Volts;

import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.plant.Wheel;
import edu.wpi.first.units.measure.Voltage;
import edu.wpi.first.wpilibj.RobotController;

/** Represents a simulated wheel mechanism controlled by voltage input. */
public class WheelSim extends WheelSimBase {
  /**
   * Creates a simulated wheel mechanism.
   *
   * @param plant The linear system that represents the wheel. This system can be created with
   *     {@link edu.wpi.first.math.system.plant.LinearSystemId#createWheelSystem(Wheel, double,
   *     int)} or {@link
   *     edu.wpi.first.math.system.plant.LinearSystemId#identifyPositionSystem(double, double)}. If
   *     {@link edu.wpi.first.math.system.plant.LinearSystemId#identifyPositionSystem(double,
   *     double)} is used, the distance unit must be meters.
   * @param wheel The wheel object containing the radius of the wheel and gearbox.
   * @param measurementStdDevs The standard deviations of the measurements. Can be omitted if no
   *     noise is desired. If present must have 2 elements. The first element is for position. The
   *     second element is for velocity.
   */
  public WheelSim(LinearSystem<N2, N1, N2> plant, Wheel wheel, double... measurementStdDevs) {
    // By theorem 6.9.1 of
    // https://file.tavsys.net/control/controls-engineering-in-frc.pdf,
    // the state-space model with voltage as input is:
    //
    // dx/dt = -G²Kₜ/(KᵥRr²m)x + (GKₜ)/(Rrm)u
    // A = -G²Kₜ/(KᵥRr²m)
    // B = (GKₜ)/(Rrm)
    //
    // Solve for m.
    //
    // B = GKₜ/(Rrm)
    // m = GKₜ/(RrB)
    super(
        plant,
        wheel,
        Kilograms.of(
            wheel.gearbox.reduction
                * wheel.gearbox.motorType.KtNMPerAmp
                / wheel.gearbox.motorType.rOhms
                / wheel.radiusMeters
                / plant.getB(1, 0)),
        measurementStdDevs);
  }

  /**
   * Sets the input voltage for the wheel.
   *
   * @param volts The input voltage.
   */
  public void setInputVoltage(double volts) {
    setInput(volts);
    clampInput(RobotController.getBatteryVoltage());
    m_voltage.mut_replace(m_u.get(0, 0), Volts);
  }

  /**
   * Sets the input voltage for the wheel.
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
        m_wheel.currentAmps(m_x.get(1, 0), getInput(0)) * Math.signum(m_u.get(0, 0)), Amps);
    m_voltage.mut_replace(getInput(0), Volts);
    m_force.mut_replace(m_wheel.forceNewtons(m_currentDraw.in(Amps)), Newtons);
    m_torque.mut_replace(m_force.in(Newtons) * m_wheel.radiusMeters, NewtonMeters);
  }
}
