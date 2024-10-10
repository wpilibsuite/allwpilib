// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static edu.wpi.first.units.Units.Inches;
import static edu.wpi.first.units.Units.KilogramSquareMeters;
import static edu.wpi.first.units.Units.Kilograms;
import static edu.wpi.first.units.Units.Meters;
import static edu.wpi.first.units.Units.Pounds;
import static edu.wpi.first.units.Units.Volts;

import edu.wpi.first.math.StateSpaceUtil;
import edu.wpi.first.math.system.plant.LinearSystemId;
import edu.wpi.first.math.system.plant.Wheel;
import edu.wpi.first.math.util.Units;
import edu.wpi.first.units.measure.Distance;
import edu.wpi.first.units.measure.Mass;
import edu.wpi.first.units.measure.MomentOfInertia;
import edu.wpi.first.wpilibj.RobotController;

/**
 * This class simulates the state of the drivetrain. In simulationPeriodic, users should first set
 * inputs from motors with {@link #setInputs(double, double)}, call {@link #update(double)} to
 * update the simulation, and set estimated encoder and gyro positions, as well as estimated
 * odometry pose. Teams can use {@link edu.wpi.first.wpilibj.smartdashboard.Field2d} to visualize
 * their robot on the Sim GUI's field.
 *
 * <p>Our state-space system is:
 *
 * <p>x = [[x, y, theta, vel_l, vel_r, dist_l, dist_r]]ᵀ in the field coordinate system (dist_* are
 * wheel distances.)
 *
 * <p>u = [[voltage_l, voltage_r]]ᵀ This is typically the control input of the last timestep from a
 * LTVDiffDriveController.
 *
 * <p>y = x
 */
public class DifferentialDrivetrainSim extends DifferentialDrivetrainSimBase {
  /**
   * Creates a simulated differential drivetrain.
   *
   * @param wheel A {@link Wheel} representing one of the drivetrain's wheel.
   * @param J The moment of inertia of the drivetrain about its center.
   * @param mass The mass of the drivebase.
   * @param trackWidth The robot's track width, or distance between left and right wheels.
   * @param measurementStdDevs Standard deviations for measurements, in the form [x, y, heading,
   *     left velocity, right velocity, left distance, right distance]ᵀ. Can be null if no noise is
   *     desired. Gyro standard deviations of 0.0001 radians, velocity standard deviations of 0.05
   *     m/s, and position measurement standard deviations of 0.005 meters are a reasonable starting
   *     point. If present must have 7 elements matching the aforementioned measurements.
   */
  public DifferentialDrivetrainSim(
      Wheel wheel,
      MomentOfInertia J,
      Mass mass,
      Distance trackWidth,
      double... measurementStdDevs) {
    super(
        LinearSystemId.createDrivetrainSystem(
            wheel, mass.in(Kilograms), J.in(KilogramSquareMeters), trackWidth.in(Meters)),
        wheel,
        J,
        mass,
        trackWidth,
        measurementStdDevs);
  }

  /**
   * Sets the gearing reduction on the drivetrain. This is commonly used for shifting drivetrains.
   *
   * @param newGearRatio The new gear ratio, as output over input.
   */
  public void setCurrentGearing(double newGearRatio) {
    m_plant
        .getA()
        .set(3, 3, m_plant.getA(3, 3) * Math.pow(newGearRatio, 2) / Math.pow(m_currentGearing, 2));
    m_plant
        .getA()
        .set(3, 4, m_plant.getA(3, 4) * Math.pow(newGearRatio, 2) / Math.pow(m_currentGearing, 2));
    m_plant
        .getA()
        .set(4, 3, m_plant.getA(4, 3) * Math.pow(newGearRatio, 2) / Math.pow(m_currentGearing, 2));
    m_plant
        .getA()
        .set(4, 4, m_plant.getA(4, 4) * Math.pow(newGearRatio, 2) / Math.pow(m_currentGearing, 2));
    m_plant.getB().set(3, 0, m_plant.getB(3, 0) * newGearRatio / m_currentGearing);
    m_plant.getB().set(3, 1, m_plant.getB(3, 1) * newGearRatio / m_currentGearing);
    m_plant.getB().set(4, 0, m_plant.getB(4, 0) * newGearRatio / m_currentGearing);
    m_plant.getB().set(4, 1, m_plant.getB(4, 1) * newGearRatio / m_currentGearing);
    m_currentGearing = newGearRatio;
  }

  /**
   * Sets the applied voltage to the drivetrain. Note that positive voltage must make that side of
   * the drivetrain travel forward (+X).
   *
   * @param leftVoltageVolts The left voltage.
   * @param rightVoltageVolts The right voltage.
   */
  public void setInputs(double leftVoltageVolts, double rightVoltageVolts) {
    m_u.set(0, 0, leftVoltageVolts);
    m_u.set(1, 0, rightVoltageVolts);
    m_u = StateSpaceUtil.desaturateInputVector(m_u, RobotController.getBatteryVoltage());
  }

  @Override
  public void update(double dtSeconds) {
    double v = (getLeftVelocityMetersPerSecond() + getRightVelocityMetersPerSecond()) / 2;
    double halfCos = Math.cos(m_x.get(2, 0)) / 2.0;
    double halfSin = Math.sin(m_x.get(2, 0)) / 2.0;
    m_plant.getA().set(0, 2, -v * Math.sin(m_x.get(2, 0)));
    m_plant.getA().set(1, 2, v * Math.cos(m_x.get(2, 0)));
    m_plant.getA().set(0, 3, halfCos);
    m_plant.getA().set(0, 4, halfCos);
    m_plant.getA().set(1, 3, halfSin);
    m_plant.getA().set(1, 4, halfSin);
    super.update(dtSeconds);
    m_leftVoltage.mut_replace(m_u.get(0, 0), Volts);
    m_rightVoltage.mut_replace(m_u.get(1, 0), Volts);
  }

  /**
   * Create a sim for the standard FRC kitbot.
   *
   * @param gearbox The gearbox installed in the bot.
   * @param gearing The gearing reduction used.
   * @param wheelSize The wheel size.
   * @param measurementStdDevs Standard deviations for measurements, in the form [x, y, heading,
   *     left velocity, right velocity, left distance, right distance]ᵀ. Can be null if no noise is
   *     desired. Gyro standard deviations of 0.0001 radians, velocity standard deviations of 0.05
   *     m/s, and position measurement standard deviations of 0.005 meters are a reasonable starting
   *     point.
   * @return A sim for the standard FRC kitbot.
   */
  public static DifferentialDrivetrainSim createKitbotSim(
      KitbotGearbox gearbox,
      KitbotGearing gearing,
      KitbotWheelSize wheelSize,
      double... measurementStdDevs) {
    // MOI estimation -- note that I = mr² for point masses
    var batteryMoi = 12.5 / 2.2 * Math.pow(Units.inchesToMeters(10), 2);
    var gearboxMoi =
        (2.8 /* CIM motor */ * 2 / 2.2 + 2.0 /* Toughbox Mini- ish */)
            * Math.pow(Units.inchesToMeters(26.0 / 2.0), 2);

    return createKitbotSim(
        gearbox, gearing, wheelSize, batteryMoi + gearboxMoi, measurementStdDevs);
  }

  /**
   * Create a sim for the standard FRC kitbot.
   *
   * @param gearbox The gearboxes installed in the bot.
   * @param gearing The gearing reduction used.
   * @param wheelSize The wheel size.
   * @param jKgMetersSquared The moment of inertia of the drivebase. This can be calculated using
   *     SysId.
   * @param measurementStdDevs Standard deviations for measurements, in the form [x, y, heading,
   *     left velocity, right velocity, left distance, right distance]ᵀ. Can be null if no noise is
   *     desired. Gyro standard deviations of 0.0001 radians, velocity standard deviations of 0.05
   *     m/s, and position measurement standard deviations of 0.005 meters are a reasonable starting
   *     point.
   * @return A sim for the standard FRC kitbot.
   */
  public static DifferentialDrivetrainSim createKitbotSim(
      KitbotGearbox gearbox,
      KitbotGearing gearing,
      KitbotWheelSize wheelSize,
      double jKgMetersSquared,
      double... measurementStdDevs) {
    return new DifferentialDrivetrainSim(
        new Wheel(gearbox.value.withReduction(gearing.value), Inches.of(wheelSize.value / 2.0)),
        KilogramSquareMeters.of(jKgMetersSquared),
        Pounds.of(60),
        Inches.of(26),
        measurementStdDevs);
  }
}
