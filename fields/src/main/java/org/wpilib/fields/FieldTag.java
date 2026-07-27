// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.fields;

import io.avaje.jsonb.Json;
import java.util.Objects;
import org.wpilib.math.geometry.Pose3d;

/** Represents a field tag's metadata. */
@Json
public class FieldTag {
  /** The tag's ID. */
  @Json.Property("ID")
  @SuppressWarnings("PMD.PublicFieldNamingConvention")
  public int ID;

  /** The tag's pose. */
  @Json.Property("pose")
  public Pose3d pose;

  /**
   * Constructs a field tag.
   *
   * @param ID The tag's ID.
   * @param pose The tag's pose.
   */
  @SuppressWarnings("ParameterName")
  @Json.Creator
  public FieldTag(int ID, Pose3d pose) {
    this.ID = ID;
    this.pose = pose;
  }

  @Override
  public boolean equals(Object obj) {
    return obj instanceof FieldTag tag && ID == tag.ID && pose.equals(tag.pose);
  }

  @Override
  public int hashCode() {
    return Objects.hash(ID, pose);
  }

  @Override
  public String toString() {
    return "FieldTag(ID: " + ID + ", pose: " + pose + ")";
  }
}
