// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static edu.wpi.first.units.Units.Amps;
import static edu.wpi.first.units.Units.Kilograms;
import static edu.wpi.first.units.Units.Meters;
import static edu.wpi.first.units.Units.MetersPerSecondPerSecond;
import static edu.wpi.first.units.Units.NewtonMeters;
import static edu.wpi.first.units.Units.Newtons;
import static edu.wpi.first.units.Units.Volts;

import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.math.system.plant.LinearSystemId;
import edu.wpi.first.units.measure.Distance;
import edu.wpi.first.units.measure.LinearAcceleration;
import edu.wpi.first.units.measure.Voltage;
import edu.wpi.first.wpilibj.RobotController;

/** Represents a simulated DC motor mechanism. */
public class ElevatorSim extends ElevatorSimBase {
  /**
   * Creates a simulated elevator mechanism.
   *
   * @param plant The linear system that represents the elevator. This system can be created with
   *     {@link edu.wpi.first.math.system.plant.LinearSystemId#createElevatorSystem(DCMotor, double,
   *     double, double)}or {@link
   *     edu.wpi.first.math.system.plant.LinearSystemId#identifyPositionSystem(double, double)}. If
   *     {@link edu.wpi.first.math.system.plant.LinearSystemId#identifyPositionSystem(double,
   *     double)} is used, the distance unit must be meters.
   * @param gearbox The type of and number of motors in the elevator gearbox.
   * @param drumRadiusMeters The radius of the elevator's drum in meters.
   * @param minHeightMeters The min allowable height of the elevator in meters.
   * @param maxHeightMeters The max allowable height of the elevator in meters.
   * @param gMetersPerSecondSquared The acceleration due to gravity in meters per second squared. To
   *     disable gravity in the simulation set to 0. The enable normal earth gravity use -9.8.
   *     Values between 0 and -9.8 can be used to simulate free fall more accurately, but must be
   *     determined by the user.
   * @param startingHeightMeters The starting height of the elevator in meters.
   * @param measurementStdDevs The standard deviations of the measurements. Can be omitted if no
   *     noise is desired. If present must have 2 elements. The first element is for position. The
   *     second element is for velocity.
   */
  public ElevatorSim(
      LinearSystem<N2, N1, N2> plant,
      DCMotor gearbox,
      double drumRadiusMeters,
      double minHeightMeters,
      double maxHeightMeters,
      double gMetersPerSecondSquared,
      double startingHeightMeters,
      double... measurementStdDevs) {
    this(
        plant,
        gearbox,
        Meters.of(drumRadiusMeters),
        Meters.of(minHeightMeters),
        Meters.of(maxHeightMeters),
        MetersPerSecondPerSecond.of(gMetersPerSecondSquared),
        Meters.of(startingHeightMeters),
        measurementStdDevs);
  }

  /**
   * Creates a simulated elevator mechanism.
   *
   * @param plant The linear system that represents the elevator. This system can be created with
   *     {@link edu.wpi.first.math.system.plant.LinearSystemId#createElevatorSystem(DCMotor, double,
   *     double, double)}or {@link
   *     edu.wpi.first.math.system.plant.LinearSystemId#identifyPositionSystem(double, double)}. If
   *     {@link edu.wpi.first.math.system.plant.LinearSystemId#identifyPositionSystem(double,
   *     double)} is used, the distance unit must be meters.
   * @param gearbox The type of and number of motors in the elevator gearbox.
   * @param drumRadius The radius of the elevator's drum.
   * @param minHeight The min allowable height of the elevator.
   * @param maxHeight The max allowable height of the elevator.
   * @param g The acceleration due to gravity in meters per second squared. To disable gravity in
   *     the simulation set to 0. The enable normal earth gravity use -9.8. Values between 0 and
   *     -9.8 can be used to simulate free fall more accurately, but must be determined by the user.
   * @param startingHeight The starting height of the elevator.
   * @param measurementStdDevs The standard deviations of the measurements. Can be omitted if no
   *     noise is desired. If present must have 2 elements. The first element is for position. The
   *     second element is for velocity.
   */
  public ElevatorSim(
      LinearSystem<N2, N1, N2> plant,
      DCMotor gearbox,
      Distance drumRadius,
      Distance minHeight,
      Distance maxHeight,
      LinearAcceleration g,
      Distance startingHeight,
      double... measurementStdDevs) {
    // By theorem 6.9.1 of
    // https://file.tavsys.net/control/controls-engineering-in-frc.pdf,
    // the elevator state-space model with voltage as input is:
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
        drumRadius,
        minHeight,
        maxHeight,
        g,
        startingHeight,
        measurementStdDevs);
  }

  /**
   * Creates a simulated elevator mechanism.
   *
   * @param kv The velocity gain of the elevator in volt / meters per second.
   * @param ka The acceleration gain of the elevator in volts / meters per second squared.
   * @param gearbox The type of and number of motors in the elevator gearbox.
   * @param drumRadius The radius of the elevator's drum.
   * @param minHeight The min allowable height of the elevator.
   * @param maxHeight The max allowable height of the elevator.
   * @param kg The gravity gain of the elevator.
   * @param startingHeight The starting height of the elevator.
   * @param measurementStdDevs The standard deviations of the measurements. Can be omitted if no
   *     noise is desired. If present must have 2 elements. The first element is for position. The
   *     second element is for velocity.
   */
  public ElevatorSim(
      double kv,
      double ka,
      DCMotor gearbox,
      Distance drumRadius,
      Distance minHeight,
      Distance maxHeight,
      Voltage kg,
      Distance startingHeight,
      double... measurementStdDevs) {
    this(
        LinearSystemId.identifyPositionSystem(kv, ka),
        gearbox,
        drumRadius,
        minHeight,
        maxHeight,
        MetersPerSecondPerSecond.of(-kg.in(Volts) / ka),
        startingHeight,
        measurementStdDevs);
  }

  /**
   * Creates a simulated elevator mechanism.
   *
   * @param kv The velocity gain of the elevator in volt / meters per second.
   * @param ka The acceleration gain of the elevator in volts / meters per second squared.
   * @param gearbox The type of and number of motors in the elevator gearbox.
   * @param drumRadiusMeters The radius of the elevator's drum in meters.
   * @param minHeightMeters The min allowable height of the elevator in meters.
   * @param maxHeightMeters The max allowable height of the elevator in meters.
   * @param kg The gravity gain of the elevator in volts.
   * @param startingHeightMeters The starting height of the elevator in meters.
   * @param measurementStdDevs The standard deviations of the measurements. Can be omitted if no
   *     noise is desired. If present must have 2 elements. The first element is for position. The
   *     second element is for velocity.
   */
  public ElevatorSim(
      double kv,
      double ka,
      DCMotor gearbox,
      double drumRadiusMeters,
      double minHeightMeters,
      double maxHeightMeters,
      double kg,
      double startingHeightMeters,
      double... measurementStdDevs) {
    this(
        kv,
        ka,
        gearbox,
        Meters.of(drumRadiusMeters),
        Meters.of(minHeightMeters),
        Meters.of(maxHeightMeters),
        Volts.of(kg),
        Meters.of(startingHeightMeters),
        measurementStdDevs);
  }

  /**
   * Sets the input voltage for the elevator.
   *
   * @param volts The input voltage.
   */
  public void setInputVoltage(double volts) {
    setInput(volts);
    clampInput(RobotController.getBatteryVoltage());
    m_voltage.mut_replace(m_u.get(0, 0), Volts);
  }

  /**
   * Sets the input voltage for the elevator.
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
                m_x.get(1, 0) * m_gearing / 2 / Math.PI / m_drumRadius.in(Meters), getInput(0))
            * Math.signum(m_u.get(0, 0)),
        Amps);
    m_voltage.mut_replace(getInput(0), Volts);
    m_torque.mut_replace(
        getMassKilograms() * getAccelerationMetersPerSecondSquared() * m_drumRadius.in(Meters),
        NewtonMeters);
    m_force.mut_replace(getMassKilograms() * getAccelerationMetersPerSecondSquared(), Newtons);
  }
}
