// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.apriltag;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;
import edu.wpi.first.apriltag.jni.AprilTagJNI;
import edu.wpi.first.math.geometry.Pose3d;
import java.util.Objects;
import org.opencv.core.Mat;

@SuppressWarnings("MemberName")
public class AprilTag {
  @JsonProperty(value = "ID")
  public int ID;

  @JsonProperty(value = "pose")
  public Pose3d pose;

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
    if (obj instanceof AprilTag) {
      var other = (AprilTag) obj;
      return ID == other.ID && pose.equals(other.pose);
    }
    return false;
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
   * Generates an opencv Mat containing the apriltag with the id with family 16h5 passed in.
   *
   * @param id id
   * @return An OpenCV Mat containing the AprilTag image
   */
  public static Mat generate16h5AprilTagImage(int id) {
    Mat generatedImage = new Mat();
    AprilTagJNI.generate16h5AprilTagImage(id, generatedImage.getNativeObjAddr());
    return generatedImage;
  }

  /**
   * Generates an opencv Mat containing the apriltag with the id with family 36h11 passed in.
   *
   * @param id id
   * @return An OpenCV Mat containing the AprilTag image
   */
  public static Mat generate36h11AprilTagImage(int id) {
    Mat generatedImage = new Mat();
    AprilTagJNI.generate36h11AprilTagImage(id, generatedImage.getNativeObjAddr());
    return generatedImage;
  }
}
