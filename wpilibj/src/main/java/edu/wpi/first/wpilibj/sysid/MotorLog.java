// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.sysid;

import static edu.wpi.first.units.Units.Meters;
import static edu.wpi.first.units.Units.MetersPerSecond;
import static edu.wpi.first.units.Units.Rotations;
import static edu.wpi.first.units.Units.RotationsPerSecond;
import static edu.wpi.first.units.Units.Volts;

import edu.wpi.first.units.Angle;
import edu.wpi.first.units.Distance;
import edu.wpi.first.units.Measure;
import edu.wpi.first.units.Velocity;
import edu.wpi.first.units.Voltage;

/** Handle for recording SysId motor data to WPILog. */
@FunctionalInterface
public interface MotorLog {
  /**
   * Records a frame of motor data during a SysId routine. Data fields are recorded as their own log
   * entries (named like "voltage-motorName-mechanismName"), and have their units specified in
   * metadata.
   *
   * @param voltage Voltage applied to the motor in volts.
   * @param distance Total displacement of the motor, in units of distanceUnit.
   * @param velocity Rate of change of displacement of the motor, in units of distanceUnit per
   *     second.
   * @param motorName Name of the motor, used as the topic name for the WPILog entry for this data
   *     frame.
   * @param distanceUnit String name of the unit used to measure distance, e.g. "meters".
   */
  void recordFrame(
      double voltage, double distance, double velocity, String motorName, String distanceUnit);

  /**
   * Records a frame of motor data during a SysId routine. Data fields are recorded as their own log
   * entries (named like "voltage-motorName-mechanismName"), and have their units specified in
   * metadata. Assumes a linear distance dimension, and records all data to WPILog in meters.
   *
   * <p>For best performance, MutableMeasure should be used by calling code to avoid the need to
   * allocate during each call.
   *
   * @param voltage Voltage applied to the motor.
   * @param distance Total linear displacement of the motor.
   * @param velocity Rate of change of linear displacement of the motor.
   * @param motorName Name of the motor, used as the topic name for the WPILog entry for this data
   *     frame.
   */
  default void recordFrameLinear(
      Measure<Voltage> voltage,
      Measure<Distance> distance,
      Measure<Velocity<Distance>> velocity,
      String motorName) {
    recordFrame(
        voltage.in(Volts),
        distance.in(Meters),
        velocity.in(MetersPerSecond),
        motorName,
        Meters.name());
  }

  /**
   * Records a frame of motor data during a SysId routine. Data fields are recorded as their own log
   * entries (named like "voltage-motorName-mechanismName"), and have their units specified in
   * metadata. Assumes an angular distance dimension, and records all data to WPILog in rotations.
   *
   * <p>For best performance, MutableMeasure should be used by calling code to avoid the need to
   * allocate during each call.
   *
   * @param voltage Voltage applied to the motor.
   * @param angle Total angular displacement of the motor.
   * @param velocity Rate of change of angular displacement of the motor.
   * @param motorName Name of the motor, used as the topic name for the WPILog entry for this data
   *     frame.
   */
  default void recordFrameAngular(
      Measure<Voltage> voltage,
      Measure<Angle> angle,
      Measure<Velocity<Angle>> velocity,
      String motorName) {
    recordFrame(
        voltage.in(Volts),
        angle.in(Rotations),
        velocity.in(RotationsPerSecond),
        motorName,
        Rotations.name());
  }
}
