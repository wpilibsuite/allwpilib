/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

import edu.wpi.first.hal.SimDevice;
import edu.wpi.first.hal.SimDouble;
import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.geometry.Translation2d;

/**
 * 2D representation of game field (for simulation).
 *
 * <p>In non-simulation mode this simply stores and returns the robot pose.
 *
 * <p>The robot pose is the actual location shown on the simulation view.
 * This may or may not match the robot's internal odometry.  For example, if
 * the robot is shown at a particular starting location, the pose in this
 * class would represent the actual location on the field, but the robot's
 * internal state might have a 0,0,0 pose (unless it's initialized to
 * something different).
 *
 * <p>As the user is able to edit the pose, code performing updates should get
 * the robot pose, transform it as appropriate (e.g. based on simulated wheel
 * velocity), and set the new pose.
 */
public class Field2d {
  public Field2d() {
    m_device = SimDevice.create("Field2D");
    if (m_device != null) {
      m_x = m_device.createDouble("x", false, 0.0);
      m_y = m_device.createDouble("y", false, 0.0);
      m_rot = m_device.createDouble("rot", false, 0.0);
    }
  }

  /**
   * Set the robot pose from a Pose object.
   *
   * @param pose 2D pose
   */
  public void setRobotPose(Pose2d pose) {
    if (m_device != null) {
      Translation2d translation = pose.getTranslation();
      m_x.set(translation.getX());
      m_y.set(translation.getY());
      m_rot.set(pose.getRotation().getDegrees());
    } else {
      m_pose = pose;
    }
  }

  /**
   * Set the robot pose from x, y, and rotation.
   *
   * @param xMeters X location, in meters
   * @param yMeters Y location, in meters
   * @param rotation rotation
   */
  public void setRobotPose(double xMeters, double yMeters, Rotation2d rotation) {
    if (m_device != null) {
      m_x.set(xMeters);
      m_y.set(yMeters);
      m_rot.set(rotation.getDegrees());
    } else {
      m_pose = new Pose2d(xMeters, yMeters, rotation);
    }
  }

  /**
   * Get the robot pose.
   *
   * @return 2D pose
   */
  public Pose2d getRobotPose() {
    if (m_device != null) {
      return new Pose2d(m_x.get(), m_y.get(), Rotation2d.fromDegrees(m_rot.get()));
    } else {
      return m_pose;
    }
  }

  private Pose2d m_pose;

  @SuppressWarnings("MemberName")
  private final SimDevice m_device;

  @SuppressWarnings("MemberName")
  private SimDouble m_x;

  @SuppressWarnings("MemberName")
  private SimDouble m_y;

  private SimDouble m_rot;
}
