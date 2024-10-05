// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static edu.wpi.first.units.Units.Amps;
import static edu.wpi.first.units.Units.Kilograms;
import static edu.wpi.first.units.Units.Meters;
import static edu.wpi.first.units.Units.NewtonMeters;
import static edu.wpi.first.units.Units.Newtons;
import static edu.wpi.first.units.Units.Volts;

import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.units.measure.Distance;
import edu.wpi.first.units.measure.Voltage;
import edu.wpi.first.wpilibj.RobotController;

/** Represents a simulated wheel mechanism controlled by voltage input. */
public class WheelSim extends WheelSimBase {
  /**
   * Creates a simulated wheel mechanism.
   *
   * @param plant The linear system that represents the wheel. This system can be created with
   *     {@link edu.wpi.first.math.system.plant.LinearSystemId #createWheelSystem(DCMotor, double,
   *     double, double)} or {@link edu.wpi.first.math.system.plant.LinearSystemId
   *     #identifyPositionSystem(double, double)}. If {@link
   *     edu.wpi.first.math.system.plant.LinearSystemId #identifyPositionSystem(double, double)} is
   *     used, the distance unit must be meters.
   * @param gearbox The type of and number of motors in the wheel gearbox.
   * @param radiusMeters The radius of the wheel in meters.
   * @param measurementStdDevs The standard deviations of the measurements. Can be omitted if no
   *     noise is desired. If present must have 2 elements. The first element is for position. The
   *     second element is for velocity.
   */
  public WheelSim(
      LinearSystem<N2, N1, N2> plant,
      DCMotor gearbox,
      double radiusMeters,
      double... measurementStdDevs) {
    this(plant, gearbox, Meters.of(radiusMeters), measurementStdDevs);
  }

  /**
   * Creates a simulated wheel mechanism.
   *
   * @param plant The linear system that represents the wheel. This system can be created with
   *     {@link edu.wpi.first.math.system.plant.LinearSystemId#createWheelSystem(DCMotor, double,
   *     double, double)}or {@link
   *     edu.wpi.first.math.system.plant.LinearSystemId#identifyPositionSystem(double, double)}. If
   *     {@link edu.wpi.first.math.system.plant.LinearSystemId#identifyPositionSystem(double,
   *     double)} is used, the distance unit must be meters.
   * @param gearbox The type of and number of motors in the wheel gearbox.
   * @param radius The radius of the wheel.
   * @param measurementStdDevs The standard deviations of the measurements. Can be omitted if no
   *     noise is desired. If present must have 2 elements. The first element is for position. The
   *     second element is for velocity.
   */
  public WheelSim(
      LinearSystem<N2, N1, N2> plant,
      DCMotor gearbox,
      Distance radius,
      double... measurementStdDevs) {
    // By theorem 6.9.1 of
    // https://file.tavsys.net/control/controls-engineering-in-frc.pdf,
    // the state-space model with voltage as input is:
    //
    // dx/dt = -G²Kₜ/(KᵥRr²m)x + (GKₜ)/(Rrm)u
    // A = -G²Kₜ/(KᵥRr²m)
    // B = (GKₜ)/(Rrm)
    //
    // Solve for G.
    //
    // A/B = -G/Kᵥr
    // G = -KᵥrA/B
    //
    // Solve for m.
    //
    // B = GKₜ/(Rrm)
    // m = GKₜ/(RrB)
    // m = -KᵥrKₜA/(RrB²)
    // m = -KᵥKₜA/(RB²)
    super(
        plant,
        gearbox,
        -gearbox.KvRadPerSecPerVolt * plant.getA(1, 1) / plant.getB(1, 0),
        Kilograms.of(
            -gearbox.KvRadPerSecPerVolt
                * gearbox.KtNMPerAmp
                * plant.getA(0, 0)
                / gearbox.rOhms
                * Math.pow(plant.getB(1, 0), 2)),
        radius,
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
    // I = V / R - omega / (Kv * R)
    // Reductions are output over input, so a reduction of 2:1 means the motor is
    // spinning
    // 2x faster than the flywheel
    m_currentDraw.mut_replace(
        m_gearbox.getCurrent(
                m_x.get(1, 0) * m_gearing / 2 / Math.PI / m_radius.in(Meters), getInput(0))
            * Math.signum(m_u.get(0, 0)),
        Amps);
    m_voltage.mut_replace(getInput(0), Volts);
    m_torque.mut_replace(
        getMassKilograms() * getAccelerationMetersPerSecondSquared() * m_radius.in(Meters),
        NewtonMeters);
    m_force.mut_replace(getMassKilograms() * getAccelerationMetersPerSecondSquared(), Newtons);
  }
}
