// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.apriltag;

import com.fasterxml.jackson.annotation.JsonAutoDetect;
import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.databind.ObjectMapper;
import edu.wpi.first.math.geometry.Pose3d;
import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.geometry.Translation3d;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Optional;

/**
 * Class for representing a layout of AprilTags on a field and reading them from a JSON format.
 *
 * <p>The JSON format contains two top-level objects, "tags" and "field". The "tags" object is a
 * list of all AprilTags contained within a layout. Each AprilTag serializes to a JSON object
 * containing an ID and a Pose3d. The "field" object is a descriptor of the size of the field in
 * meters with "width" and "length" values. This is to account for arbitrary field sizes when
 * transforming the poses.
 *
 * <p>Pose3ds are assumed to be measured from the bottom-left corner of the field, when the blue
 * alliance is at the left. By default, Pose3ds will be returned as declared when calling {@link
 * AprilTagFieldLayout#getTagPose(int)}. {@link #setOrigin(OriginPosition)} can be used to transform
 * the poses returned from {@link AprilTagFieldLayout#getTagPose(int)} to be correct relative to a
 * different coordinate frame.
 */
@JsonIgnoreProperties(ignoreUnknown = true)
@JsonAutoDetect(getterVisibility = JsonAutoDetect.Visibility.NONE)
public class AprilTagFieldLayout {
  public enum OriginPosition {
    kBlueAllianceWallRightSide,
    kRedAllianceWallRightSide,
  }

  private final Map<Integer, AprilTag> m_apriltags = new HashMap<>();

  @JsonProperty(value = "field")
  private FieldDimensions m_fieldDimensions;

  private Pose3d m_origin;

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
    m_apriltags.putAll(layout.m_apriltags);
    m_fieldDimensions = layout.m_fieldDimensions;
    setOrigin(OriginPosition.kBlueAllianceWallRightSide);
  }

  /**
   * Construct a new AprilTagFieldLayout from a list of {@link AprilTag} objects.
   *
   * @param apriltags List of {@link AprilTag}.
   * @param fieldLength Length of the field the layout is representing in meters.
   * @param fieldWidth Width of the field the layout is representing in meters.
   */
  public AprilTagFieldLayout(List<AprilTag> apriltags, double fieldLength, double fieldWidth) {
    this(apriltags, new FieldDimensions(fieldLength, fieldWidth));
  }

  @JsonCreator
  private AprilTagFieldLayout(
      @JsonProperty(required = true, value = "tags") List<AprilTag> apriltags,
      @JsonProperty(required = true, value = "field") FieldDimensions fieldDimensions) {
    // To ensure the underlying semantics don't change with what kind of list is passed in
    for (AprilTag tag : apriltags) {
      m_apriltags.put(tag.ID, tag);
    }
    m_fieldDimensions = fieldDimensions;
    setOrigin(OriginPosition.kBlueAllianceWallRightSide);
  }

  /**
   * Returns a List of the {@link AprilTag AprilTags} used in this layout.
   *
   * @return The {@link AprilTag AprilTags} used in this layout.
   */
  @JsonProperty("tags")
  public List<AprilTag> getTags() {
    return new ArrayList<>(m_apriltags.values());
  }

  /**
   * Sets the origin based on a predefined enumeration of coordinate frame origins. The origins are
   * calculated from the field dimensions.
   *
   * <p>This transforms the Pose3ds returned by {@link #getTagPose(int)} to return the correct pose
   * relative to a predefined coordinate frame.
   *
   * @param origin The predefined origin
   */
  @JsonIgnore
  public void setOrigin(OriginPosition origin) {
    switch (origin) {
      case kBlueAllianceWallRightSide:
        setOrigin(new Pose3d());
        break;
      case kRedAllianceWallRightSide:
        setOrigin(
            new Pose3d(
                new Translation3d(m_fieldDimensions.fieldLength, m_fieldDimensions.fieldWidth, 0),
                new Rotation3d(0, 0, Math.PI)));
        break;
      default:
        throw new IllegalArgumentException("Unsupported enum value");
    }
  }

  /**
   * Sets the origin for tag pose transformation.
   *
   * <p>This transforms the Pose3ds returned by {@link #getTagPose(int)} to return the correct pose
   * relative to the provided origin.
   *
   * @param origin The new origin for tag transformations
   */
  @JsonIgnore
  public void setOrigin(Pose3d origin) {
    m_origin = origin;
  }

  /**
   * Gets an AprilTag pose by its ID.
   *
   * @param ID The ID of the tag.
   * @return The pose corresponding to the ID passed in or an empty optional if a tag with that ID
   *     was not found.
   */
  @SuppressWarnings("ParameterName")
  public Optional<Pose3d> getTagPose(int ID) {
    AprilTag tag = m_apriltags.get(ID);
    if (tag == null) {
      return Optional.empty();
    }
    return Optional.of(tag.pose.relativeTo(m_origin));
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

  /**
   * Deserializes a field layout from a resource within a jar file.
   *
   * @param resourcePath The absolute path of the resource
   * @return The deserialized layout
   * @throws IOException If the resource could not be loaded
   */
  public static AprilTagFieldLayout loadFromResource(String resourcePath) throws IOException {
    try (InputStream stream = AprilTagFieldLayout.class.getResourceAsStream(resourcePath);
        InputStreamReader reader = new InputStreamReader(stream)) {
      return new ObjectMapper().readerFor(AprilTagFieldLayout.class).readValue(reader);
    }
  }

  @Override
  public boolean equals(Object obj) {
    if (obj instanceof AprilTagFieldLayout) {
      var other = (AprilTagFieldLayout) obj;
      return m_apriltags.equals(other.m_apriltags) && m_origin.equals(other.m_origin);
    }
    return false;
  }

  @Override
  public int hashCode() {
    return Objects.hash(m_apriltags, m_origin);
  }

  @JsonIgnoreProperties(ignoreUnknown = true)
  @JsonAutoDetect(getterVisibility = JsonAutoDetect.Visibility.NONE)
  private static class FieldDimensions {
    @SuppressWarnings("MemberName")
    @JsonProperty(value = "length")
    public double fieldLength;

    @SuppressWarnings("MemberName")
    @JsonProperty(value = "width")
    public double fieldWidth;

    @JsonCreator()
    FieldDimensions(
        @JsonProperty(required = true, value = "length") double fieldLength,
        @JsonProperty(required = true, value = "width") double fieldWidth) {
      this.fieldLength = fieldLength;
      this.fieldWidth = fieldWidth;
    }
  }
}
