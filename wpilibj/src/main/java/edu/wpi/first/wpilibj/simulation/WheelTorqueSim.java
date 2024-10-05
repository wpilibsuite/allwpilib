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
import edu.wpi.first.units.measure.Torque;

/** Represents a simulated wheel mechanism controlled by torque input. */
public class WheelTorqueSim extends WheelSimBase {
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
   * @param gearing The gearing from the motors to the output.
   * @param radiusMeters The radius of the wheel in meters.
   * @param measurementStdDevs The standard deviations of the measurements. Can be omitted if no
   *     noise is desired. If present must have 2 elements. The first element is for position. The
   *     second element is for velocity.
   */
  public WheelTorqueSim(
      LinearSystem<N2, N1, N2> plant,
      DCMotor gearbox,
      double gearing,
      double radiusMeters,
      double... measurementStdDevs) {
    this(plant, gearbox, gearing, Meters.of(radiusMeters), measurementStdDevs);
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
   * @param gearing The gearing from the motors to the output.
   * @param radius The radius of the wheel.
   * @param measurementStdDevs The standard deviations of the measurements. Can be omitted if no
   *     noise is desired. If present must have 2 elements. The first element is for position. The
   *     second element is for velocity.
   */
  public WheelTorqueSim(
      LinearSystem<N2, N1, N2> plant,
      DCMotor gearbox,
      double gearing,
      Distance radius,
      double... measurementStdDevs) {
    // τ = rF, F = ma, τ = rma.
    // The acceleration a = τ / mr
    // the wheel state-space model with torque as input is:
    //
    // dx/dt = 0 x + (1/mr) u
    // A = 0
    // B = (1/mr)
    //
    // Solve for m.
    //
    // B = (1/mr)
    // m = (1/Br)
    super(
        plant,
        gearbox,
        gearing,
        Kilograms.of(1.0 / radius.in(Meters) / plant.getB(1, 0)),
        radius,
        measurementStdDevs);
  }

  /**
   * Sets the input torque for the wheel.
   *
   * @param torqueNM The input torque.
   */
  public void setInputTorque(double torqueNM) {
    setInput(torqueNM);
    // TODO: Need some guidance on clamping.
    m_torque.mut_replace(m_u.get(0, 0), NewtonMeters);
  }

  /**
   * Sets the input torque for the wheel.
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
        m_gearbox.getCurrent(getInput(0)) * m_gearing * Math.signum(m_u.get(0, 0)), Amps);
    m_voltage.mut_replace(
        m_gearbox.getVoltage(getInput(0), m_x.get(1, 0) / m_radius.in(Meters)), Volts);
    m_torque.mut_replace(getInput(0), NewtonMeters);
    m_force.mut_replace(getInput(0) / m_radius.in(Meters), Newtons);
  }
}
