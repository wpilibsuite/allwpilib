/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.smartdashboard;

import java.util.ArrayList;
import java.util.List;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.wpilibj.Sendable;
import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;

/**
 * 2D representation of game field for dashboards.
 *
 * <p>An object's pose is the location shown on the dashboard view.  Note that
 * for the robot, this may or may not match the internal odometry.  For example,
 * the robot is shown at a particular starting location, the pose in this
 * class would represent the actual location on the field, but the robot's
 * internal state might have a 0,0,0 pose (unless it's initialized to
 * something different).
 *
 * <p>As the user is able to edit the pose, code performing updates should get
 * the robot pose, transform it as appropriate (e.g. based on wheel odometry),
 * and set the new pose.
 *
 * <p>This class provides methods to set the robot pose, but other objects can
 * also be shown by using the getObject() function.  Other objects can
 * also have multiple poses (which will show the object at multiple locations).
 */
public class Field2d implements Sendable {
  /**
   * Constructor.
   */
  public Field2d() {
    FieldObject2d obj = new FieldObject2d("Robot");
    obj.setPose(new Pose2d());
    m_objects.add(obj);
  }

  /**
   * Set the robot pose from a Pose object.
   *
   * @param pose 2D pose
   */
  public synchronized void setRobotPose(Pose2d pose) {
    m_objects.get(0).setPose(pose);
  }

  /**
   * Set the robot pose from x, y, and rotation.
   *
   * @param xMeters X location, in meters
   * @param yMeters Y location, in meters
   * @param rotation rotation
   */
  @SuppressWarnings("ParameterName")
  public synchronized void setRobotPose(double xMeters, double yMeters, Rotation2d rotation) {
    m_objects.get(0).setPose(xMeters, yMeters, rotation);
  }

  /**
   * Get the robot pose.
   *
   * @return 2D pose
   */
  public synchronized Pose2d getRobotPose() {
    return m_objects.get(0).getPose();
  }

  /**
   * Get or create a field object.
   *
   * @return Field object
   */
  public synchronized FieldObject2d getObject(String name) {
    for (FieldObject2d obj : m_objects) {
      if (obj.m_name == name) {
        return obj;
      }
    }
    FieldObject2d obj = new FieldObject2d(name);
    m_objects.add(obj);
    if (m_table != null) {
      synchronized (obj) {
        obj.m_entry = m_table.getEntry(name);
      }
    }
    return obj;
  }

  /**
   * Get the robot object.
   *
   * @return Field object for robot
   */
  public synchronized FieldObject2d getRobotObject() {
    return m_objects.get(0);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Field2d");
    m_table = builder.getTable();

    synchronized (this) {
      for (FieldObject2d obj : m_objects) {
        synchronized (obj) {
          obj.m_entry = m_table.getEntry(obj.m_name);
          obj.updateEntry(true);
        }
      }
    }
  }

  private NetworkTable m_table;
  private final List<FieldObject2d> m_objects = new ArrayList<>();
}
