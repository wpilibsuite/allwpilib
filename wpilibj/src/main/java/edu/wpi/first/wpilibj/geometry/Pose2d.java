/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.geometry;

/**
 * Represents a 2d pose containing translational and rotational elements.
 */
public class Pose2d {
  private final Translation2d m_translation;
  private final Rotation2d m_rotation;

  /**
   * Constructs a pose at the origin facing toward the positive X axis.
   * (Translation2d{0, 0} and Rotation{0})
   */
  public Pose2d() {
    m_translation = new Translation2d();
    m_rotation = new Rotation2d();
  }

  /**
   * Constructs a pose with the specified translation and rotation.
   *
   * @param translation The translational component of the pose.
   * @param rotation    The rotational component of the pose.
   */
  public Pose2d(Translation2d translation, Rotation2d rotation) {
    m_translation = translation;
    m_rotation = rotation;
  }

  /**
   * Convenience constructors that takes in x and y values directly instead of
   * having to construct a Translation2d.
   *
   * @param x        The x component of the translational component of the pose.
   * @param y        The y component of the translational component of the pose.
   * @param rotation The rotational component of the pose.
   */
  @SuppressWarnings("ParameterName")
  public Pose2d(double x, double y, Rotation2d rotation) {
    m_translation = new Translation2d(x, y);
    m_rotation = rotation;
  }

  /**
   * Transforms the pose by the given transformation and returns the new
   * transformed pose.
   *
   * <p>The matrix multiplication is as follows
   * [x_new] += [cos, -sin, 0][transform.x]
   * [y_new] += [sin,  cos, 0][transform.y]
   * [t_new] += [0,    0,   1][transform.t]
   *
   * @param other The transform to transform the pose by.
   * @return The transformed pose.
   */
  public Pose2d plus(Transform2d other) {
    return transformBy(other);
  }

  /**
   * Returns the translation component of the transformation.
   *
   * @return The translational component of the pose.
   */
  public Translation2d getTranslation() {
    return m_translation;
  }

  /**
   * Returns the rotational component of the transformation.
   *
   * @return The rotational component of the pose.
   */
  public Rotation2d getRotation() {
    return m_rotation;
  }

  /**
   * Transforms the pose by the given transformation and returns the new pose.
   * See + operator for the matrix multiplication performed.
   *
   * @param other The transform to transform the pose by.
   * @return The transformed pose.
   */
  public Pose2d transformBy(Transform2d other) {
    return new Pose2d(m_translation.plus(other.getTranslation().rotateBy(m_rotation)),
            m_rotation.plus(other.getRotation()));
  }

  /**
   * Returns the other pose relative to the current pose.
   *
   * <p>This function can often be used for trajectory tracking or pose
   * stabilization algorithms to get the error between the reference and the
   * current pose.
   *
   * @param other The pose that is the origin of the new coordinate frame that
   *              the current pose will be converted into.
   * @return The current pose relative to the new origin pose.
   */
  public Pose2d relativeTo(Pose2d other) {
    var transform = new Transform2d(other, this);
    return new Pose2d(transform.getTranslation(), transform.getRotation());
  }

  /**
   * Obtain a new Pose2d from a (constant curvature) velocity.
   *
   * <p>See <a href="https://file.tavsys.net/control/state-space-guide.pdf">
   * Controls Engineering in the FIRST Robotics Competition</a>
   * section on nonlinear pose estimation for derivation.
   *
   * <p>The twist is a change in pose in the robot's coordinate frame since the
   * previous pose update. When the user runs exp() on the previous known
   * field-relative pose with the argument being the twist, the user will
   * receive the new field-relative pose.
   *
   * <p>"Exp" represents the pose exponential, which is solving a differential
   * equation moving the pose forward in time.
   *
   * @param twist The change in pose in the robot's coordinate frame since the
   *              previous pose update. For example, if a non-holonomic robot moves forward
   *              0.01 meters and changes angle by .5 degrees since the previous pose update,
   *              the twist would be Twist2d{0.01, 0.0, toRadians(0.5)}
   * @return The new pose of the robot.
   */
  @SuppressWarnings("LocalVariableName")
  public Pose2d exp(Twist2d twist) {
    double dx = twist.dx;
    double dy = twist.dy;
    double dtheta = twist.dtheta;

    double sinTheta = Math.sin(dtheta);
    double cosTheta = Math.cos(dtheta);

    double s;
    double c;
    if (Math.abs(dtheta) < 1E-9) {
      s = 1.0 - 1.0 / 6.0 * dtheta * dtheta;
      c = 0.5 * dtheta;
    } else {
      s = sinTheta / dtheta;
      c = (1 - cosTheta) / dtheta;
    }
    var transform = new Transform2d(new Translation2d(dx * s - dy * c, dx * c + dy * s),
            new Rotation2d(cosTheta, sinTheta));

    return this.plus(transform);
  }
}
