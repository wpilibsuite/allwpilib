// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.fields;

import io.avaje.jsonb.Json;
import java.util.Objects;
import org.wpilib.math.geometry.Pose3d;

/** Represents a field tag's metadata. */
@Json
public final class FieldTag {
  @Json.Property("ID")
  private final int m_id;

  @Json.Property("pose")
  private final Pose3d m_pose;

  /**
   * Constructs a field tag.
   *
   * @param ID The tag's ID.
   * @param pose The tag's pose.
   */
  @SuppressWarnings("ParameterName")
  @Json.Creator
  public FieldTag(int ID, Pose3d pose) {
    m_id = ID;
    m_pose = Objects.requireNonNull(pose, "pose");
  }

  /**
   * Returns the tag's ID.
   *
   * @return The tag's ID.
   */
  public int getID() {
    return m_id;
  }

  /**
   * Returns the tag's pose.
   *
   * @return The tag's pose.
   */
  public Pose3d getPose() {
    return m_pose;
  }

  @Override
  public boolean equals(Object obj) {
    return obj instanceof FieldTag tag && m_id == tag.m_id && m_pose.equals(tag.m_pose);
  }

  @Override
  public int hashCode() {
    return Objects.hash(m_id, m_pose);
  }

  @Override
  public String toString() {
    return "FieldTag(ID: " + m_id + ", pose: " + m_pose + ")";
  }
}
