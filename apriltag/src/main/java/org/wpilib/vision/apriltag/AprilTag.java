// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.vision.apriltag;

import io.avaje.jsonb.Json;
import java.util.Objects;
import org.wpilib.math.geometry.Pose3d;
import org.wpilib.util.RawFrame;
import org.wpilib.vision.apriltag.jni.AprilTagJNI;

/** Represents an AprilTag's metadata. */
@SuppressWarnings("MemberName")
@Json
public class AprilTag {
  /** The tag's ID. */
  @Json.Property("ID")
  public int ID;

  /** The tag's pose. */
  @Json.Property("pose")
  public Pose3d pose;

  /**
   * Constructs an AprilTag.
   *
   * @param ID The tag's ID.
   * @param pose The tag's pose.
   */
  @SuppressWarnings("ParameterName")
  @Json.Creator
  public AprilTag(int ID, Pose3d pose) {
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
