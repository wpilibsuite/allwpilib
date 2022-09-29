// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.apriltag;

import com.fasterxml.jackson.annotation.JsonAutoDetect;
import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.databind.ObjectMapper;
import edu.wpi.first.math.geometry.Pose3d;
import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.geometry.Translation3d;
import edu.wpi.first.wpilibj.DriverStation;
import java.io.IOException;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

/**
 * Class for representing a layout of AprilTags on a field and reading them from a JSON format.
 *
 * <p>The JSON format contains two top-level objects, "tags" and "field". The "tags" object is a
 * list of all AprilTags contained within a layout. Each AprilTag serializes to a JSON object
 * containing an ID and a Pose3d. The "field" object is a descriptor of the size of the field in
 * meters with "width" and "height" values. This is to account for arbitrary field sizes when
 * mirroring the poses.
 *
 * <p>Pose3ds are assumed to be measured from the bottom-left corner of the field, when the blue
 * alliance is at the left. Pose3ds will automatically be returned as passed in when calling {@link
 * AprilTagFieldLayout#getTagPose(int)}. Setting an alliance color via {@link
 * AprilTagFieldLayout#setAlliance(DriverStation.Alliance)} will mirror the poses returned from
 * {@link AprilTagFieldLayout#getTagPose(int)} to be correct relative to the other alliance.
 */
@JsonIgnoreProperties(ignoreUnknown = true)
@JsonAutoDetect(getterVisibility = JsonAutoDetect.Visibility.NONE)
public class AprilTagFieldLayout {
  @JsonProperty(value = "tags")
  private final List<AprilTag> m_apriltags = new ArrayList<>();

  @JsonProperty(value = "field")
  private FieldDimensions m_fieldDimensions;

  private boolean m_mirror;

  /**
   * Construct a new AprilTagFieldLayout with values imported from a JSON file.
   *
   * @param path Path of the JSON file to import from.
   * @throws IOException If reading from the file fails.
   */
  public AprilTagFieldLayout(String path) throws IOException {
    this(Path.of(path));
  }

  /**
   * Construct a new AprilTagFieldLayout with values imported from a JSON file.
   *
   * @param path Path of the JSON file to import from.
   * @throws IOException If reading from the file fails.
   */
  public AprilTagFieldLayout(Path path) throws IOException {
    AprilTagFieldLayout layout =
        new ObjectMapper().readValue(path.toFile(), AprilTagFieldLayout.class);
    m_apriltags.addAll(layout.m_apriltags);
    m_fieldDimensions = layout.m_fieldDimensions;
  }

  /**
   * Construct a new AprilTagFieldLayout from a list of {@link AprilTag} objects.
   *
   * @param apriltags List of {@link AprilTag}.
   * @param fieldLength Length of the field in meters.
   * @param fieldWidth Width of the field in meters.
   */
  public AprilTagFieldLayout(List<AprilTag> apriltags, double fieldLength, double fieldWidth) {
    this(apriltags, new FieldDimensions(fieldLength, fieldWidth));
  }

  @JsonCreator
  private AprilTagFieldLayout(
      @JsonProperty(required = true, value = "tags") List<AprilTag> apriltags,
      @JsonProperty(required = true, value = "field") FieldDimensions fieldDimensions) {
    // To ensure the underlying semantics don't change with what kind of list is passed in
    m_apriltags.addAll(apriltags);
    m_fieldDimensions = fieldDimensions;
  }

  /**
   * Set the alliance that your team is on.
   *
   * <p>This changes the {@link #getTagPose(int)} method to return the correct pose for your
   * alliance.
   *
   * @param alliance The alliance to mirror poses for.
   */
  public void setAlliance(DriverStation.Alliance alliance) {
    m_mirror = alliance == DriverStation.Alliance.Red;
  }

  /**
   * Gets an AprilTag pose by its ID.
   *
   * @param ID The ID of the tag.
   * @return The pose corresponding to the ID passed in.
   */
  @SuppressWarnings("ParameterName")
  public Pose3d getTagPose(int ID) {
    Pose3d pose = m_apriltags.stream().filter(it -> ID == it.ID).findFirst().get().pose;
    if (m_mirror) {
      pose =
          pose.relativeTo(
              new Pose3d(
                  new Translation3d(
                      m_fieldDimensions.fieldWidth, m_fieldDimensions.fieldLength, 0.0),
                  new Rotation3d(0.0, 0.0, Math.PI)));
    }

    return pose;
  }

  /**
   * Serializes a AprilTagFieldLayout to a JSON file.
   *
   * @param path The path to write to.
   * @throws IOException If writing to the file fails.
   */
  public void serialize(String path) throws IOException {
    serialize(Path.of(path));
  }

  /**
   * Serializes a AprilTagFieldLayout to a JSON file.
   *
   * @param path The path to write to.
   * @throws IOException If writing to the file fails.
   */
  public void serialize(Path path) throws IOException {
    new ObjectMapper().writeValue(path.toFile(), this);
  }

  @Override
  public boolean equals(Object obj) {
    if (obj instanceof AprilTagFieldLayout) {
      var other = (AprilTagFieldLayout) obj;
      return m_apriltags.equals(other.m_apriltags) && m_mirror == other.m_mirror;
    }
    return false;
  }

  @Override
  public int hashCode() {
    return Objects.hash(m_apriltags, m_mirror);
  }

  @JsonIgnoreProperties(ignoreUnknown = true)
  @JsonAutoDetect(getterVisibility = JsonAutoDetect.Visibility.NONE)
  private static class FieldDimensions {
    @SuppressWarnings("MemberName")
    @JsonProperty(value = "width")
    public double fieldWidth;

    @SuppressWarnings("MemberName")
    @JsonProperty(value = "height")
    public double fieldLength;

    @JsonCreator()
    FieldDimensions(
        @JsonProperty(required = true, value = "width") double fieldWidth,
        @JsonProperty(required = true, value = "height") double fieldLength) {
      this.fieldWidth = fieldWidth;
      this.fieldLength = fieldLength;
    }
  }
}
