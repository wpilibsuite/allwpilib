// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static edu.wpi.first.units.Units.Amps;
import static edu.wpi.first.units.Units.KilogramSquareMeters;
import static edu.wpi.first.units.Units.Meters;
import static edu.wpi.first.units.Units.MetersPerSecondPerSecond;
import static edu.wpi.first.units.Units.Radians;
import static edu.wpi.first.units.Units.RadiansPerSecond;
import static edu.wpi.first.units.Units.RadiansPerSecondPerSecond;
import static edu.wpi.first.units.Units.Volts;

import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.plant.Gearbox;
import edu.wpi.first.math.system.plant.LinearSystemId;
import edu.wpi.first.units.AngularAccelerationUnit;
import edu.wpi.first.units.AngularVelocityUnit;
import edu.wpi.first.units.Measure;
import edu.wpi.first.units.PerUnit;
import edu.wpi.first.units.VoltageUnit;
import edu.wpi.first.units.measure.Angle;
import edu.wpi.first.units.measure.Distance;
import edu.wpi.first.units.measure.LinearAcceleration;
import edu.wpi.first.units.measure.Voltage;
import edu.wpi.first.wpilibj.RobotController;

/** Represents a simulated arm mechanism controlled by voltage. */
public class SingleJointedArmSim extends SingleJointedArmSimBase {
  /**
   * Creates a simulated arm mechanism.
   *
   * @param plant The linear system that represents the arm. This system can be created with {@link
   *     edu.wpi.first.math.system.plant.LinearSystemId#createSingleJointedArmSystem(Gearbox,
   *     double, double, double)} or {@link
   *     edu.wpi.first.math.system.plant.LinearSystemId#identifyPositionSystem(double, double)}. If
   *     {@link edu.wpi.first.math.system.plant.LinearSystemId#identifyPositionSystem(double,
   *     double)} is used, the distance unit must be meters.
   * @param gearbox The type of and number of motors in the arm gearbox.
   * @param armLength The length of the arm.
   * @param pivotDistance The distance of the pivot from the center of mass.
   * @param minAngle The minimum angle that the arm is capable of.
   * @param maxAngle The maximum angle that the arm is capable of.
   * @param g The acceleration due to gravity in meters per second squared. To disable gravity in
   *     the simulation set to 0. The enable normal earth gravity use -9.8. Values between 0 and
   *     -9.8 can be used to simulate free fall more accurately, but must be determined by the user.
   * @param startingAngle The initial position of the Arm simulation.
   * @param measurementStdDevs The standard deviations of the measurements. Can be omitted if no
   *     noise is desired. If present must have 1 element for position.
   */
  public SingleJointedArmSim(
      LinearSystem<N2, N1, N2> plant,
      Gearbox gearbox,
      Distance armLength,
      Distance pivotDistance,
      Angle minAngle,
      Angle maxAngle,
      LinearAcceleration g,
      Angle startingAngle,
      double... measurementStdDevs) {
    // By theorem 6.10.1 of
    // https://file.tavsys.net/control/controls-engineering-in-frc.pdf,
    // the DC motor mechanism state-space model with voltage as input is:
    //
    // dx/dt = -G²Kₜ/(KᵥRJ)x + (GKₜ)/(RJ)u
    // A = -G²Kₜ/(KᵥRJ)
    // B = GKₜ/(RJ)
    //
    // Solve for J.
    //
    // B = GKₜ/(RJ)
    // J = GKₜ/(RB)
    super(
        plant,
        gearbox,
        armLength,
        pivotDistance,
        KilogramSquareMeters.of(
            gearbox.numMotors
                * gearbox.reduction
                * gearbox.motorType.KtNMPerAmp
                / gearbox.motorType.rOhms
                / plant.getB(1, 0)),
        minAngle,
        maxAngle,
        g,
        startingAngle,
        measurementStdDevs);
  }

  /**
   * Creates a simulated arm mechanism.
   *
   * @param plant The linear system that represents the arm. This system can be created with {@link
   *     edu.wpi.first.math.system.plant.LinearSystemId#createSingleJointedArmSystem(Gearbox,
   *     double, double, double)} or {@link
   *     edu.wpi.first.math.system.plant.LinearSystemId#identifyPositionSystem(double, double)}. If
   *     {@link edu.wpi.first.math.system.plant.LinearSystemId#identifyPositionSystem(double,
   *     double)} is used, the distance unit must be meters.
   * @param gearbox The type of and number of motors in the arm gearbox.
   * @param armLengthMeters The length of the arm.
   * @param pivotDistanceMeters The distance of the pivot from the center of mass.
   * @param minAngleRadians The minimum angle that the arm is capable of.
   * @param maxAngleRadians The maximum angle that the arm is capable of.
   * @param gMetersPerSecondSquared The acceleration due to gravity in meters per second squared. To
   *     disable gravity in the simulation set to 0. The enable normal earth gravity use -9.8.
   *     Values between 0 and -9.8 can be used to simulate free fall more accurately, but must be
   *     determined by the user. * @param startingAngleRadians The initial position of the Arm
   *     simulation.
   * @param startingAngleRadians The initial position of the Arm simulation.
   * @param measurementStdDevs The standard deviations of the measurements. Can be omitted if no
   *     noise is desired. If present must have 1 element for position.
   */
  public SingleJointedArmSim(
      LinearSystem<N2, N1, N2> plant,
      Gearbox gearbox,
      double armLengthMeters,
      double pivotDistanceMeters,
      double minAngleRadians,
      double maxAngleRadians,
      double gMetersPerSecondSquared,
      double startingAngleRadians,
      double... measurementStdDevs) {
    this(
        plant,
        gearbox,
        Meters.of(armLengthMeters),
        Meters.of(pivotDistanceMeters),
        Radians.of(minAngleRadians),
        Radians.of(maxAngleRadians),
        MetersPerSecondPerSecond.of(gMetersPerSecondSquared),
        Radians.of(startingAngleRadians),
        measurementStdDevs);
  }

  /**
   * Creates a simulated arm mechanism.
   *
   * @param kv The velocity gain of the arm.
   * @param ka The acceleration gain of the arm.
   * @param kg The gravity gain of the arm.
   * @param gearbox The type of and number of motors in the arm gearbox.
   * @param armLength The length of the arm.
   * @param pivotDistance The distance of the pivot from the center of mass.
   * @param minAngle The minimum angle that the arm is capable of.
   * @param maxAngle The maximum angle that the arm is capable of.
   * @param startingAngle The initial position of the Arm simulation.
   * @param measurementStdDevs The standard deviations of the measurements. Can be omitted if no
   *     noise is desired. If present must have 1 element for position.
   */
  public SingleJointedArmSim(
      Measure<? extends PerUnit<VoltageUnit, AngularVelocityUnit>> kv,
      Measure<? extends PerUnit<VoltageUnit, AngularAccelerationUnit>> ka,
      Voltage kg,
      Gearbox gearbox,
      Distance armLength,
      Distance pivotDistance,
      Angle minAngle,
      Angle maxAngle,
      Angle startingAngle,
      double... measurementStdDevs) {
    super(
        LinearSystemId.identifyPositionSystem(kv.baseUnitMagnitude(), ka.baseUnitMagnitude()),
        kg,
        gearbox,
        armLength,
        pivotDistance,
        minAngle,
        maxAngle,
        startingAngle,
        measurementStdDevs);
  }

  /**
   * Creates a simulated arm mechanism.
   *
   * @param kv The velocity gain of the arm.
   * @param ka The acceleration gain of the arm.
   * @param kg The gravity gain of the arm.
   * @param gearbox The type of and number of motors in the arm gearbox.
   * @param armLengthMeters The length of the arm.
   * @param pivotDistanceMeters The distance of the pivot from the center of mass.
   * @param minAngleRadians The minimum angle that the arm is capable of.
   * @param maxAngleRadians The maximum angle that the arm is capable of.
   * @param startingAngleRadians The initial position of the Arm simulation.
   * @param measurementStdDevs The standard deviations of the measurements. Can be omitted if no
   *     noise is desired. If present must have 1 element for position.
   */
  public SingleJointedArmSim(
      double kv,
      double ka,
      double kg,
      Gearbox gearbox,
      double armLengthMeters,
      double pivotDistanceMeters,
      double minAngleRadians,
      double maxAngleRadians,
      double startingAngleRadians,
      double... measurementStdDevs) {
    this(
        Volts.per(RadiansPerSecond).of(kv),
        Volts.per(RadiansPerSecondPerSecond).of(ka),
        Volts.of(kg),
        gearbox,
        Meters.of(armLengthMeters),
        Meters.of(pivotDistanceMeters),
        Radians.of(minAngleRadians),
        Radians.of(maxAngleRadians),
        Radians.of(startingAngleRadians),
        measurementStdDevs);
  }

  /**
   * Sets the input voltage for the arm.
   *
   * @param volts The input voltage.
   */
  public void setInputVoltage(double volts) {
    setInput(volts);
    clampInput(RobotController.getBatteryVoltage());
    m_voltage.mut_replace(m_u.get(0, 0), Volts);
  }

  /**
   * Sets the input voltage for the arm.
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
        m_gearbox.currentAmps(m_x.get(1, 0), getInput(0)) * Math.signum(m_u.get(0, 0)), Amps);
    m_voltage.mut_replace(getInput(0), Volts);
    m_torque.mut_replace(m_gearbox.torque(m_currentDraw));
  }
}
