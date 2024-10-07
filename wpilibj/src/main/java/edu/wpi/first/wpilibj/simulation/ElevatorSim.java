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
import static edu.wpi.first.units.Units.VoltsPerMeterPerSecondSquared;

import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.math.system.plant.LinearSystemId;
import edu.wpi.first.math.system.plant.Wheel;
import edu.wpi.first.units.LinearAccelerationUnit;
import edu.wpi.first.units.LinearVelocityUnit;
import edu.wpi.first.units.VoltageUnit;
import edu.wpi.first.units.measure.Distance;
import edu.wpi.first.units.measure.LinearAcceleration;
import edu.wpi.first.units.measure.Per;
import edu.wpi.first.units.measure.Voltage;
import edu.wpi.first.wpilibj.RobotController;

/** Represents a simulated elevator mechanism controlled by voltage. */
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
   * @param drum The elevator's drum.
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
      Wheel drum,
      double minHeightMeters,
      double maxHeightMeters,
      double gMetersPerSecondSquared,
      double startingHeightMeters,
      double... measurementStdDevs) {
    this(
        plant,
        drum,
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
   * @param drum The elevator's drum.
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
      Wheel drum,
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
    super(
        plant,
        drum,
        Kilograms.of(
            drum.gearbox.reduction
                * drum.gearbox.motorType.KtNMPerAmp
                / drum.gearbox.motorType.rOhms
                / drum.radiusMeters
                / plant.getB(1, 0)),
        minHeight,
        maxHeight,
        g,
        startingHeight,
        measurementStdDevs);
  }

  /**
   * Creates a simulated elevator mechanism.
   *
   * @param kv The velocity gain of the elevator.
   * @param ka The acceleration gain of the elevator.
   * @param kg The gravity gain of the elevator.
   * @param drum The elevator's drum.
   * @param minHeight The min allowable height of the elevator.
   * @param maxHeight The max allowable height of the elevator.
   * @param startingHeight The starting height of the elevator.
   * @param measurementStdDevs The standard deviations of the measurements. Can be omitted if no
   *     noise is desired. If present must have 2 elements. The first element is for position. The
   *     second element is for velocity.
   */
  public ElevatorSim(
      Per<VoltageUnit, LinearVelocityUnit> kv,
      Per<VoltageUnit, LinearAccelerationUnit> ka,
      Voltage kg,
      Wheel drum,
      Distance minHeight,
      Distance maxHeight,
      Distance startingHeight,
      double... measurementStdDevs) {
    this(
        LinearSystemId.identifyPositionSystem(kv.baseUnitMagnitude(), ka.baseUnitMagnitude()),
        drum,
        minHeight,
        maxHeight,
        MetersPerSecondPerSecond.of(-kg.in(Volts) / ka.in(VoltsPerMeterPerSecondSquared)),
        startingHeight,
        measurementStdDevs);
  }

  /**
   * Creates a simulated elevator mechanism.
   *
   * @param kv The velocity gain of the elevator in volt / meters per second.
   * @param ka The acceleration gain of the elevator in volts / meters per second squared.
   * @param kg The gravity gain of the elevator in volts.
   * @param drum The elevator's drum.
   * @param minHeightMeters The min allowable height of the elevator in meters.
   * @param maxHeightMeters The max allowable height of the elevator in meters.
   * @param startingHeightMeters The starting height of the elevator in meters.
   * @param measurementStdDevs The standard deviations of the measurements. Can be omitted if no
   *     noise is desired. If present must have 2 elements. The first element is for position. The
   *     second element is for velocity.
   */
  public ElevatorSim(
      double kv,
      double ka,
      double kg,
      Wheel drum,
      double minHeightMeters,
      double maxHeightMeters,
      double startingHeightMeters,
      double... measurementStdDevs) {
    this(
        LinearSystemId.identifyPositionSystem(kv, ka),
        drum,
        minHeightMeters,
        maxHeightMeters,
        -kg / ka,
        startingHeightMeters,
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
    m_currentDraw.mut_replace(
        m_drum.currentAmps(m_x.get(1, 0), getInput(0)) * Math.signum(m_u.get(0, 0)), Amps);
    m_voltage.mut_replace(getInput(0), Volts);
    m_force.mut_replace(m_drum.forceNewtons(m_currentDraw.in(Amps)), Newtons);
    m_torque.mut_replace(m_force.in(Newtons) * m_drum.radiusMeters, NewtonMeters);
  }
}
