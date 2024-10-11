// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static edu.wpi.first.units.Units.NewtonMeters;
import static edu.wpi.first.units.Units.Volts;

import edu.wpi.first.math.StateSpaceUtil;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.plant.Wheel;
import edu.wpi.first.units.measure.Voltage;
import edu.wpi.first.wpilibj.RobotController;

/**
 * This class simulates the state of the drivetrain controlled by voltage. In simulationPeriodic,
 * users should first set inputs from motors with {@link #setInputVoltages(double, double)}, call
 * {@link #update(double)} to update the simulation, and set estimated encoder and gyro positions,
 * as well as estimated odometry pose. Teams can use {@link
 * edu.wpi.first.wpilibj.smartdashboard.Field2d} to visualize their robot on the Sim GUI's field.
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
   * @param plant The {@link LinearSystem} representing the robot's drivetrain. This system can be
   *     created with {@link
   *     edu.wpi.first.math.system.plant.LinearSystemId#createDrivetrainVelocitySystem(Wheel,
   *     double, double, double)} or {@link
   *     edu.wpi.first.math.system.plant.LinearSystemId#identifyDrivetrainSystem(double, double,
   *     double, double)}.
   * @param driveWheel A {@link Wheel} representing one of the drivetrain's wheels.
   * @param trackWidthMeters The distance between the two sides of the drivetrain. Can be found with
   *     SysId.
   * @param measurementStdDevs The standard deviations of the measurements, in the form [x, y,
   *     heading, left velocity, right velocity, left distance, right distance]ᵀ. Can be omitted if
   *     no noise is desired. Gyro standard deviations of 0.0001 radians, velocity standard
   *     deviations of 0.05 m/s, and position measurement standard deviations of 0.005 meters are a
   *     reasonable starting point. * If present must have 7 elements.
   */
  public DifferentialDrivetrainSim(
      LinearSystem<N2, N2, N2> plant,
      Wheel driveWheel,
      double trackWidthMeters,
      double... measurementStdDevs) {
    super(plant, driveWheel, trackWidthMeters, measurementStdDevs);
  }

  /**
   * Sets the applied voltage to the drivetrain. Note that positive voltage must make that side of
   * the drivetrain travel forward (+X).
   *
   * @param leftVoltageVolts The left voltage.
   * @param rightVoltageVolts The right voltage.
   */
  public void setInputVoltages(double leftVoltageVolts, double rightVoltageVolts) {
    m_u.set(0, 0, leftVoltageVolts);
    m_u.set(1, 0, rightVoltageVolts);
    m_u = StateSpaceUtil.desaturateInputVector(m_u, RobotController.getBatteryVoltage());
  }

  /**
   * Sets the applied voltage to the drivetrain. Note that positive voltage must make that side of
   * the drivetrain travel forward (+X).
   *
   * @param leftVoltage The left voltage.
   * @param rightVoltage The right voltage.
   */
  public void setInputVoltages(Voltage leftVoltage, Voltage rightVoltage) {
    setInputVoltages(leftVoltage.in(Volts), rightVoltage.in(Volts));
  }

  @Override
  public void update(double dtSeconds) {
    super.update(dtSeconds);
    m_leftVoltage.mut_replace(m_u.get(0, 0), Volts);
    m_rightVoltage.mut_replace(m_u.get(1, 0), Volts);
    m_leftTorque.mut_replace(0.0, NewtonMeters); // TODO: do something here.
    m_rightTorque.mut_replace(0.0, NewtonMeters); // TODO: do something here.
  }
}
