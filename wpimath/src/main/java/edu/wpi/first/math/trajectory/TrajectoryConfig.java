// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.trajectory;

import static edu.wpi.first.units.Units.MetersPerSecond;
import static edu.wpi.first.units.Units.MetersPerSecondPerSecond;

import edu.wpi.first.math.kinematics.DifferentialDriveKinematics;
import edu.wpi.first.math.kinematics.MecanumDriveKinematics;
import edu.wpi.first.math.kinematics.SwerveDriveKinematics;
import edu.wpi.first.math.trajectory.constraint.DifferentialDriveKinematicsConstraint;
import edu.wpi.first.math.trajectory.constraint.MecanumDriveKinematicsConstraint;
import edu.wpi.first.math.trajectory.constraint.SwerveDriveKinematicsConstraint;
import edu.wpi.first.math.trajectory.constraint.TrajectoryConstraint;
import edu.wpi.first.units.measure.LinearAcceleration;
import edu.wpi.first.units.measure.LinearVelocity;
import java.util.ArrayList;
import java.util.List;

/**
 * Represents the configuration for generating a trajectory. This class stores the start velocity,
 * end velocity, max velocity, max acceleration, custom constraints, and the reversed flag.
 *
 * <p>The class must be constructed with a max velocity and max acceleration. The other parameters
 * (start velocity, end velocity, constraints, reversed) have been defaulted to reasonable values
 * (0, 0, {}, false). These values can be changed via the setXXX methods.
 */
public class TrajectoryConfig {
  private final double m_maxVelocity;
  private final double m_maxAcceleration;
  private final List<TrajectoryConstraint> m_constraints;
  private double m_startVelocity;
  private double m_endVelocity;
  private boolean m_reversed;

  /**
   * Constructs the trajectory configuration class.
   *
   * @param maxVelocityMetersPerSecond The max velocity for the trajectory.
   * @param maxAccelerationMetersPerSecondSq The max acceleration for the trajectory.
   */
  public TrajectoryConfig(
      double maxVelocityMetersPerSecond, double maxAccelerationMetersPerSecondSq) {
    m_maxVelocity = maxVelocityMetersPerSecond;
    m_maxAcceleration = maxAccelerationMetersPerSecondSq;
    m_constraints = new ArrayList<>();
  }

  /**
   * Constructs the trajectory configuration class.
   *
   * @param maxVelocity The max velocity for the trajectory.
   * @param maxAcceleration The max acceleration for the trajectory.
   */
  public TrajectoryConfig(LinearVelocity maxVelocity, LinearAcceleration maxAcceleration) {
    this(maxVelocity.in(MetersPerSecond), maxAcceleration.in(MetersPerSecondPerSecond));
  }

  /**
   * Adds a user-defined constraint to the trajectory.
   *
   * @param constraint The user-defined constraint.
   * @return Instance of the current config object.
   */
  public TrajectoryConfig addConstraint(TrajectoryConstraint constraint) {
    m_constraints.add(constraint);
    return this;
  }

  /**
   * Adds all user-defined constraints from a list to the trajectory.
   *
   * @param constraints List of user-defined constraints.
   * @return Instance of the current config object.
   */
  public TrajectoryConfig addConstraints(List<? extends TrajectoryConstraint> constraints) {
    m_constraints.addAll(constraints);
    return this;
  }

  /**
   * Adds a differential drive kinematics constraint to ensure that no wheel velocity of a
   * differential drive goes above the max velocity.
   *
   * @param kinematics The differential drive kinematics.
   * @return Instance of the current config object.
   */
  public TrajectoryConfig setKinematics(DifferentialDriveKinematics kinematics) {
    addConstraint(new DifferentialDriveKinematicsConstraint(kinematics, m_maxVelocity));
    return this;
  }

  /**
   * Adds a mecanum drive kinematics constraint to ensure that no wheel velocity of a mecanum drive
   * goes above the max velocity.
   *
   * @param kinematics The mecanum drive kinematics.
   * @return Instance of the current config object.
   */
  public TrajectoryConfig setKinematics(MecanumDriveKinematics kinematics) {
    addConstraint(new MecanumDriveKinematicsConstraint(kinematics, m_maxVelocity));
    return this;
  }

  /**
   * Adds a swerve drive kinematics constraint to ensure that no wheel velocity of a swerve drive
   * goes above the max velocity.
   *
   * @param kinematics The swerve drive kinematics.
   * @return Instance of the current config object.
   */
  public TrajectoryConfig setKinematics(SwerveDriveKinematics kinematics) {
    addConstraint(new SwerveDriveKinematicsConstraint(kinematics, m_maxVelocity));
    return this;
  }

  /**
   * Returns the starting velocity of the trajectory.
   *
   * @return The starting velocity of the trajectory.
   */
  public double getStartVelocity() {
    return m_startVelocity;
  }

  /**
   * Sets the start velocity of the trajectory.
   *
   * @param startVelocityMetersPerSecond The start velocity of the trajectory.
   * @return Instance of the current config object.
   */
  public TrajectoryConfig setStartVelocity(double startVelocityMetersPerSecond) {
    m_startVelocity = startVelocityMetersPerSecond;
    return this;
  }

  /**
   * Sets the start velocity of the trajectory.
   *
   * @param startVelocity The start velocity of the trajectory.
   * @return Instance of the current config object.
   */
  public TrajectoryConfig setStartVelocity(LinearVelocity startVelocity) {
    return setStartVelocity(startVelocity.in(MetersPerSecond));
  }

  /**
   * Returns the starting velocity of the trajectory.
   *
   * @return The starting velocity of the trajectory.
   */
  public double getEndVelocity() {
    return m_endVelocity;
  }

  /**
   * Sets the end velocity of the trajectory.
   *
   * @param endVelocityMetersPerSecond The end velocity of the trajectory.
   * @return Instance of the current config object.
   */
  public TrajectoryConfig setEndVelocity(double endVelocityMetersPerSecond) {
    m_endVelocity = endVelocityMetersPerSecond;
    return this;
  }

  /**
   * Sets the end velocity of the trajectory.
   *
   * @param endVelocity The end velocity of the trajectory.
   * @return Instance of the current config object.
   */
  public TrajectoryConfig setEndVelocity(LinearVelocity endVelocity) {
    return setEndVelocity(endVelocity.in(MetersPerSecond));
  }

  /**
   * Returns the maximum velocity of the trajectory.
   *
   * @return The maximum velocity of the trajectory.
   */
  public double getMaxVelocity() {
    return m_maxVelocity;
  }

  /**
   * Returns the maximum acceleration of the trajectory.
   *
   * @return The maximum acceleration of the trajectory.
   */
  public double getMaxAcceleration() {
    return m_maxAcceleration;
  }

  /**
   * Returns the user-defined constraints of the trajectory.
   *
   * @return The user-defined constraints of the trajectory.
   */
  public List<TrajectoryConstraint> getConstraints() {
    return m_constraints;
  }

  /**
   * Returns whether the trajectory is reversed or not.
   *
   * @return whether the trajectory is reversed or not.
   */
  public boolean isReversed() {
    return m_reversed;
  }

  /**
   * Sets the reversed flag of the trajectory.
   *
   * @param reversed Whether the trajectory should be reversed or not.
   * @return Instance of the current config object.
   */
  public TrajectoryConfig setReversed(boolean reversed) {
    m_reversed = reversed;
    return this;
  }
}
