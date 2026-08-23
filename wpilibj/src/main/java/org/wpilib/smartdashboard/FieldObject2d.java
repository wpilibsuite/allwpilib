// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.smartdashboard;

import static org.wpilib.units.Units.Meters;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.trajectory.HolonomicSample;
import org.wpilib.math.trajectory.Trajectory;
import org.wpilib.tunable.Tunable;
import org.wpilib.tunable.TunableConfig;
import org.wpilib.tunable.TunableOption;
import org.wpilib.units.measure.Distance;

/** Game field object on a Field2d. */
public class FieldObject2d implements AutoCloseable {
  private static final TunableConfig POSES_TUNABLE_CONFIG =
      TunableConfig.of(TunableOption.GET_ON_CHANGE);

  /**
   * Package-local constructor.
   *
   * @param name name
   */
  FieldObject2d(String name, Pose2d... initialPoses) {
    m_name = name;
    m_posesTunable =
        Tunable.createConfig(
            this::getPoseArray, this::setPoseArray, Pose2d[].class, POSES_TUNABLE_CONFIG);
    setPoseArray(initialPoses);
  }

  @Override
  public void close() {}

  /**
   * Set the pose from a Pose object.
   *
   * @param pose 2D pose
   */
  public synchronized void setPose(Pose2d pose) {
    setPoses(pose);
  }

  /**
   * Set the pose from x, y, and rotation.
   *
   * @param x X location, in meters
   * @param y Y location, in meters
   * @param rotation rotation
   */
  public synchronized void setPose(double x, double y, Rotation2d rotation) {
    setPose(new Pose2d(x, y, rotation));
  }

  /**
   * Set the pose from x, y, and rotation.
   *
   * @param x X location
   * @param y Y location
   * @param rotation rotation
   */
  public synchronized void setPose(Distance x, Distance y, Rotation2d rotation) {
    setPose(new Pose2d(x.in(Meters), y.in(Meters), rotation));
  }

  /**
   * Get the pose.
   *
   * @return 2D pose
   */
  public synchronized Pose2d getPose() {
    if (m_poses.isEmpty()) {
      return Pose2d.ZERO;
    }
    return m_poses.get(0);
  }

  /**
   * Set multiple poses from a list of Pose objects. The total number of poses is limited to 85.
   *
   * @param poses list of 2D poses
   */
  public synchronized void setPoses(List<Pose2d> poses) {
    m_posesTunable.set(poses.toArray(new Pose2d[0]));
  }

  /**
   * Set multiple poses from a list of Pose objects. The total number of poses is limited to 85.
   *
   * @param poses list of 2D poses
   */
  public synchronized void setPoses(Pose2d... poses) {
    m_posesTunable.set(poses.clone());
  }

  /**
   * Sets poses from a trajectory.
   *
   * @param <SampleType> The type of the trajectory sample.
   * @param trajectory The trajectory from which the poses should be added.
   */
  public synchronized <SampleType extends HolonomicSample> void setTrajectory(
      Trajectory<SampleType> trajectory) {
    List<SampleType> samples = trajectory.getSamples();
    Pose2d[] poses = new Pose2d[samples.size()];
    for (int i = 0; i < samples.size(); i++) {
      poses[i] = samples.get(i).pose;
    }
    m_posesTunable.set(poses);
  }

  /**
   * Get multiple poses.
   *
   * @return list of 2D poses
   */
  public synchronized List<Pose2d> getPoses() {
    return new ArrayList<>(m_poses);
  }

  private synchronized Pose2d[] getPoseArray() {
    return m_poses.toArray(new Pose2d[0]);
  }

  private synchronized void setPoseArray(Pose2d[] poses) {
    m_poses.clear();
    Collections.addAll(m_poses, poses);
  }

  final String m_name;
  final List<Pose2d> m_poses = new ArrayList<>();
  final Tunable<Pose2d[]> m_posesTunable;
}
