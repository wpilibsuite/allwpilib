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
  private Translation2d m_translation;
  private Rotation2d m_rotation;

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
   */
  public Pose2d(Translation2d translation, Rotation2d rotation) {
    m_translation = translation;
    m_rotation = rotation;
  }

  /**
   * Convenience constructors that takes in x and y values directly instead of
   * having to construct a Translation2d.
   */
  public Pose2d(double x, double y, Rotation2d rotation) {
    m_translation = new Translation2d(x, y);
    m_rotation = rotation;
  }

  /**
   * Transforms the pose by the given transformation and returns the new
   * transformed pose.
   * <p>
   * [x_new] += [cos, -sin, 0][transform.x]
   * [y_new] += [sin,  cos, 0][transform.y]
   * [t_new] += [0,    0,   1][transform.t]
   */
  Pose2d plus(Transform2d other) {
    return transformBy(other);
  }

  /**
   * Returns the translation component of the transformation.
   */
  public Translation2d getTranslation() {
    return m_translation;
  }

  /**
   * Returns the rotational component of the transformation.
   */
  public Rotation2d getRotation() {
    return m_rotation;
  }

  /**
   * Transforms the pose by the given transformation and returns the new pose.
   * See + operator for the matrix multiplication performed.
   */
  Pose2d transformBy(Transform2d other) {
    return new Pose2d(m_translation.plus(other.getTranslation().RotateBy(m_rotation)),
            m_rotation.plus(other.getRotation()));
  }

  /**
   * Returns the other pose relative to the current pose.
   * <p>
   * This function can often be used for trajectory tracking or pose
   * stabilization algorithms to get the error between the reference and the
   * current pose.
   */
  Pose2d relativeTo(Pose2d other) {
    var transform = new Transform2d(other, this);
    return new Pose2d(transform.getTranslation(), transform.getRotation());
  }

  /**
   * Obtain a new Pose2d from a (constant curvature) velocity.
   * <p>
   * See <https://file.tavsys.net/control/state-space-guide.pdf> section on
   * nonlinear pose estimation for derivation.
   */
  Pose2d exp(Twist2d twist) {
    double dx = twist.dx, dy = twist.dy, dtheta = twist.dtheta;

    double sinTheta = Math.sin(dtheta);
    double cosTheta = Math.cos(dtheta);

    double s, c;
    if (Math.abs(dtheta) < 1E-9) {
      s = 1.0 - 1.0 / 6.0 * dtheta * dtheta;
      c = 0.5 * dtheta;
    } else {
      s = sinTheta / dtheta;
      c = (1 - cosTheta) / dtheta;
    }
    Transform2d transform = new Transform2d(new Translation2d(dx * s - dy * c, dx * c + dy * s), new Rotation2d(cosTheta, sinTheta));

    return this.plus(transform);
  }
}
