/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.geometry;

import java.util.Objects;

/**
 * Represents a transformation for a Pose2d.
 */
public class Transform2d {
  private final Translation2d m_translation;
  private final Rotation2d m_rotation;

  /**
   * Constructs the transform that maps the initial pose to the final pose.
   *
   * @param initial The initial pose for the transformation.
   * @param last    The final pose for the transformation.
   */
  public Transform2d(Pose2d initial, Pose2d last) {
    // We are rotating the difference between the translations
    // using a clockwise rotation matrix. This transforms the global
    // delta into a local delta (relative to the initial pose).
    m_translation = last.getTranslation().minus(initial.getTranslation())
            .rotateBy(initial.getRotation().unaryMinus());

    m_rotation = last.getRotation().minus(initial.getRotation());
  }

  /**
   * Constructs a transform with the given translation and rotation components.
   *
   * @param translation Translational component of the transform.
   * @param rotation    Rotational component of the transform.
   */
  public Transform2d(Translation2d translation, Rotation2d rotation) {
    m_translation = translation;
    m_rotation = rotation;
  }

  /**
   * Constructs the identity transform -- maps an initial pose to itself.
   */
  public Transform2d() {
    m_translation = new Translation2d();
    m_rotation = new Rotation2d();
  }

  /**
   * Scales the transform by the scalar.
   *
   * @param scalar The scalar.
   * @return The scaled Transform2d.
   */
  public Transform2d times(double scalar) {
    return new Transform2d(m_translation.times(scalar), m_rotation.times(scalar));
  }

  /**
   * Returns the translation component of the transformation.
   *
   * @return The translational component of the transform.
   */
  public Translation2d getTranslation() {
    return m_translation;
  }

  /**
   * Returns the rotational component of the transformation.
   *
   * @return Reference to the rotational component of the transform.
   */
  public Rotation2d getRotation() {
    return m_rotation;
  }

  @Override
  public String toString() {
    return String.format("Transform2d(%s, %s)", m_translation, m_rotation);
  }

  /**
   * Checks equality between this Transform2d and another object.
   *
   * @param obj The other object.
   * @return Whether the two objects are equal or not.
   */
  @Override
  public boolean equals(Object obj) {
    if (obj instanceof Transform2d) {
      return ((Transform2d) obj).m_translation.equals(m_translation)
          && ((Transform2d) obj).m_rotation.equals(m_rotation);
    }
    return false;
  }

  @Override
  public int hashCode() {
    return Objects.hash(m_translation, m_rotation);
  }
}
