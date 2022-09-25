package edu.wpi.first.wpilibj.apriltag;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;

import java.util.Objects;

import edu.wpi.first.math.geometry.Pose3d;

public class AprilTag {
  public int id;
  public Pose3d pose;

  @JsonCreator
  public AprilTag(
      @JsonProperty(required = true, value = "id") int id,
      @JsonProperty(required = true, value = "pose") Pose3d pose) {
    this.id = id;
    this.pose = pose;
  }

  @Override
  public boolean equals(Object obj) {
    if (obj instanceof AprilTag) {
      var other = (AprilTag) obj;
      return id == other.id && pose.equals(other.pose);
    }
    return false;
  }

  @Override
  public int hashCode() {
    return Objects.hash(id, pose);
  }

  @Override
  public String toString() {
    return "AprilTag(id: " + id + ", pose: " + pose + ")";
  }
}
