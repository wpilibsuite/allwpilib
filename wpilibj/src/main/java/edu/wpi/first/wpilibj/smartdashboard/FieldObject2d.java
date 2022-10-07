// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.smartdashboard;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.trajectory.Trajectory;
import edu.wpi.first.networktables.NetworkTableEntry;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;

/** Game field object on a Field2d. */
public class FieldObject2d {
  /**
   * Package-local constructor.
   *
   * @param name name
   */
  FieldObject2d(String name) {
    m_name = name;
  }

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
   * @param xMeters X location, in meters
   * @param yMeters Y location, in meters
   * @param rotation rotation
   */
  public synchronized void setPose(double xMeters, double yMeters, Rotation2d rotation) {
    setPose(new Pose2d(xMeters, yMeters, rotation));
  }

  /**
   * Get the pose.
   *
   * @return 2D pose
   */
  public synchronized Pose2d getPose() {
    updateFromEntry();
    if (m_poses.isEmpty()) {
      return new Pose2d();
    }
    return m_poses.get(0);
  }

  /**
   * Set multiple poses from an list of Pose objects. The total number of poses is limited to 85.
   *
   * @param poses list of 2D poses
   */
  public synchronized void setPoses(List<Pose2d> poses) {
    m_poses.clear();
    for (Pose2d pose : poses) {
      m_poses.add(pose);
    }
    updateEntry();
  }

  /**
   * Set multiple poses from an list of Pose objects. The total number of poses is limited to 85.
   *
   * @param poses list of 2D poses
   */
  public synchronized void setPoses(Pose2d... poses) {
    m_poses.clear();
    for (Pose2d pose : poses) {
      m_poses.add(pose);
    }
    updateEntry();
  }

  /**
   * Sets poses from a trajectory.
   *
   * @param trajectory The trajectory from which the poses should be added.
   */
  public synchronized void setTrajectory(Trajectory trajectory) {
    m_poses.clear();
    for (Trajectory.State state : trajectory.getStates()) {
      m_poses.add(state.poseMeters);
    }
    updateEntry();
  }

  /**
   * Get multiple poses.
   *
   * @return list of 2D poses
   */
  public synchronized List<Pose2d> getPoses() {
    updateFromEntry();
    return new ArrayList<Pose2d>(m_poses);
  }

  void updateEntry() {
    updateEntry(false);
  }

  synchronized void updateEntry(boolean setDefault) {
    if (m_entry == null) {
      return;
    }

    if (m_poses.size() < (255 / 3)) {
      double[] arr = new double[m_poses.size() * 3];
      int ndx = 0;
      for (Pose2d pose : m_poses) {
        Translation2d translation = pose.getTranslation();
        arr[ndx + 0] = translation.getX();
        arr[ndx + 1] = translation.getY();
        arr[ndx + 2] = pose.getRotation().getDegrees();
        ndx += 3;
      }

      if (setDefault) {
        m_entry.setDefaultDoubleArray(arr);
      } else {
        m_entry.setDoubleArray(arr);
      }
    } else {
      // send as raw array of doubles if too big for NT array
      ByteBuffer output = ByteBuffer.allocate(m_poses.size() * 3 * 8);
      output.order(ByteOrder.BIG_ENDIAN);

      for (Pose2d pose : m_poses) {
        Translation2d translation = pose.getTranslation();
        output.putDouble(translation.getX());
        output.putDouble(translation.getY());
        output.putDouble(pose.getRotation().getDegrees());
      }

      if (setDefault) {
        m_entry.setDefaultRaw(output.array());
      } else {
        m_entry.forceSetRaw(output.array());
      }
    }
  }

  private synchronized void updateFromEntry() {
    if (m_entry == null) {
      return;
    }

    double[] arr = m_entry.getDoubleArray((double[]) null);
    if (arr != null) {
      if ((arr.length % 3) != 0) {
        return;
      }

      m_poses.clear();
      for (int i = 0; i < arr.length; i += 3) {
        m_poses.add(new Pose2d(arr[i], arr[i + 1], Rotation2d.fromDegrees(arr[i + 2])));
      }
    } else {
      // read as raw array of doubles
      byte[] data = m_entry.getRaw((byte[]) null);
      if (data == null) {
        return;
      }

      // must be triples of doubles
      if ((data.length % (3 * 8)) != 0) {
        return;
      }
      ByteBuffer input = ByteBuffer.wrap(data);
      input.order(ByteOrder.BIG_ENDIAN);

      m_poses.clear();
      for (int i = 0; i < (data.length / (3 * 8)); i++) {
        double x = input.getDouble();
        double y = input.getDouble();
        double rot = input.getDouble();
        m_poses.add(new Pose2d(x, y, Rotation2d.fromDegrees(rot)));
      }
    }
  }

  String m_name;
  NetworkTableEntry m_entry;
  private final List<Pose2d> m_poses = new ArrayList<>();
}
