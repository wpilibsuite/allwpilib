/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.smartdashboard;

import java.util.ArrayList;
import java.util.List;

import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.geometry.Translation2d;

/**
 * Game field object on a Field2d.
 */
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
  @SuppressWarnings("ParameterName")
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
   * Set multiple poses from an list of Pose objects.
   * The total number of poses is limited to 85.
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
   * Set multiple poses from an list of Pose objects.
   * The total number of poses is limited to 85.
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
  }

  @SuppressWarnings("PMD.AvoidInstantiatingObjectsInLoops")
  private synchronized void updateFromEntry() {
    if (m_entry == null) {
      return;
    }

    double[] arr = m_entry.getDoubleArray((double[]) null);
    if (arr == null) {
      return;
    }

    if ((arr.length % 3) != 0) {
      return;
    }

    m_poses.clear();
    for (int i = 0; i < arr.length; i += 3) {
      m_poses.add(new Pose2d(arr[i], arr[i + 1], Rotation2d.fromDegrees(arr[i + 2])));
    }
  }

  String m_name;
  NetworkTableEntry m_entry;
  private final List<Pose2d> m_poses = new ArrayList<>();
}
