// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.smartdashboard;

import static org.wpilib.units.Units.Meters;

import java.util.ArrayList;
import java.util.List;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.telemetry.TelemetryLoggable;
import org.wpilib.telemetry.TelemetryTable;
import org.wpilib.units.measure.Distance;

/**
 * 2D representation of game field for dashboards.
 *
 * <p>An object's pose is the location shown on the dashboard view. Note that for the robot, this
 * may or may not match the internal odometry. For example, the robot is shown at a particular
 * starting location, the pose in this class would represent the actual location on the field, but
 * the robot's internal state might have a 0,0,0 pose (unless it's initialized to something
 * different).
 *
 * <p>As the user is able to edit the pose, code performing updates should get the robot pose,
 * transform it as appropriate (e.g. based on wheel odometry), and set the new pose.
 *
 * <p>This class provides methods to set the robot pose, but other objects can also be shown by
 * using the getObject() function. Other objects can also have multiple poses (which will show the
 * object at multiple locations).
 */
public class Field2d implements TelemetryLoggable, AutoCloseable {
  /** Constructor. */
  public Field2d() {
    FieldObject2d obj = new FieldObject2d("Robot");
    obj.setPose(Pose2d.kZero);
    m_objects.add(obj);
  }

  @Override
  public void close() {
    for (FieldObject2d obj : m_objects) {
      obj.close();
    }
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
   * @param x X location, in meters
   * @param y Y location, in meters
   * @param rotation rotation
   */
  public synchronized void setRobotPose(double x, double y, Rotation2d rotation) {
    m_objects.get(0).setPose(x, y, rotation);
  }

  /**
   * Set the robot pose from x, y, and rotation.
   *
   * @param x X location
   * @param y Y location
   * @param rotation rotation
   */
  public synchronized void setRobotPose(Distance x, Distance y, Rotation2d rotation) {
    m_objects.get(0).setPose(x.in(Meters), y.in(Meters), rotation);
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
   * @param name The field object's name.
   * @return Field object
   */
  public synchronized FieldObject2d getObject(String name) {
    for (FieldObject2d obj : m_objects) {
      if (obj.m_name.equals(name)) {
        return obj;
      }
    }
    FieldObject2d obj = new FieldObject2d(name);
    m_objects.add(obj);
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
  public void updateTelemetry(TelemetryTable table) {
    synchronized (this) {
      for (FieldObject2d obj : m_objects) {
        synchronized (obj) {
          table.log(obj.m_name, obj.m_poses.toArray(new Pose2d[0]));
        }
      }
    }
  }

  @Override
  public String getTelemetryType() {
    return "Field2d";
  }

  private final List<FieldObject2d> m_objects = new ArrayList<>();
}
