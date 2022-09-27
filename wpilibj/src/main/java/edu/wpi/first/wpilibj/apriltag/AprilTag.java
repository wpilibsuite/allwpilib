// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.apriltag;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;
import edu.wpi.first.math.geometry.Pose3d;
import java.util.Objects;

public class AprilTag {
  @SuppressWarnings("MemberName")
  @JsonProperty(value = "ID")
  public int m_ID;

  @JsonProperty(value = "pose")
  public Pose3d m_pose;

  @SuppressWarnings("ParameterName")
  @JsonCreator
  public AprilTag(
      @JsonProperty(required = true, value = "ID") int ID,
      @JsonProperty(required = true, value = "pose") Pose3d pose) {
    this.m_ID = ID;
    this.m_pose = pose;
  }

  @Override
  public boolean equals(Object obj) {
    if (obj instanceof AprilTag) {
      var other = (AprilTag) obj;
      return m_ID == other.m_ID && m_pose.equals(other.m_pose);
    }
    return false;
  }

  @Override
  public int hashCode() {
    return Objects.hash(m_ID, m_pose);
  }

  @Override
  public String toString() {
    return "AprilTag(ID: " + m_ID + ", pose: " + m_pose + ")";
  }
}
