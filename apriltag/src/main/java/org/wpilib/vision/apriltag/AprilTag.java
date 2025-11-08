// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.apriltag;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;
import edu.wpi.first.apriltag.jni.AprilTagJNI;
import edu.wpi.first.math.geometry.Pose3d;
import edu.wpi.first.util.RawFrame;
import java.util.Objects;

/** Represents an AprilTag's metadata. */
@SuppressWarnings("MemberName")
public class AprilTag {
  /** The tag's ID. */
  @JsonProperty(value = "ID")
  public int ID;

  /** The tag's pose. */
  @JsonProperty(value = "pose")
  public Pose3d pose;

  /**
   * Constructs an AprilTag.
   *
   * @param ID The tag's ID.
   * @param pose The tag's pose.
   */
  @SuppressWarnings("ParameterName")
  @JsonCreator
  public AprilTag(
      @JsonProperty(required = true, value = "ID") int ID,
      @JsonProperty(required = true, value = "pose") Pose3d pose) {
    this.ID = ID;
    this.pose = pose;
  }

  @Override
  public boolean equals(Object obj) {
    return obj instanceof AprilTag tag && ID == tag.ID && pose.equals(tag.pose);
  }

  @Override
  public int hashCode() {
    return Objects.hash(ID, pose);
  }

  @Override
  public String toString() {
    return "AprilTag(ID: " + ID + ", pose: " + pose + ")";
  }

  /**
   * Generates a RawFrame containing the apriltag with the id with family 16h5 passed in.
   *
   * @param id id
   * @return A RawFrame containing the AprilTag image
   */
  public static RawFrame generate16h5AprilTagImage(int id) {
    RawFrame frame = new RawFrame();
    AprilTagJNI.generate16h5AprilTagImage(frame, frame.getNativeObj(), id);
    return frame;
  }

  /**
   * Generates a RawFrame containing the apriltag with the id with family 36h11 passed in.
   *
   * @param id id
   * @return A RawFrame containing the AprilTag image
   */
  public static RawFrame generate36h11AprilTagImage(int id) {
    RawFrame frame = new RawFrame();
    AprilTagJNI.generate36h11AprilTagImage(frame, frame.getNativeObj(), id);
    return frame;
  }
}
