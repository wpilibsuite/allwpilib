// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.apriltag;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;
import edu.wpi.first.math.geometry.Pose3d;
import java.util.Objects;

public class AprilTag {
  @JsonProperty(value = "id")
  public int m_id;

  @JsonProperty(value = "pose")
  public Pose3d m_pose;

  @JsonCreator
  public AprilTag(
      @JsonProperty(required = true, value = "id") int id,
      @JsonProperty(required = true, value = "pose") Pose3d pose) {
    this.m_id = id;
    this.m_pose = pose;
  }

  @Override
  public boolean equals(Object obj) {
    if (obj instanceof AprilTag) {
      var other = (AprilTag) obj;
      return m_id == other.m_id && m_pose.equals(other.m_pose);
    }
    return false;
  }

  @Override
  public int hashCode() {
    return Objects.hash(m_id, m_pose);
  }

  @Override
  public String toString() {
    return "AprilTag(id: " + m_id + ", pose: " + m_pose + ")";
  }
}
