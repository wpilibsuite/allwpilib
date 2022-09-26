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
import edu.wpi.first.math.util.Units;
import edu.wpi.first.wpilibj.DriverStation;
import java.io.IOException;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

@JsonIgnoreProperties(ignoreUnknown = true)
@JsonAutoDetect(getterVisibility = JsonAutoDetect.Visibility.NONE)
public class AprilTagFieldLayout {
  @JsonProperty(value = "tags")
  private final List<AprilTag> m_apriltags = new ArrayList<>();

  private boolean m_mirror;

  /**
   * Construct a new AprilTagFieldLayout with values imported from a JSON file.
   *
   * @param path path of the JSON file to import from
   * @throws IOException if reading from the file fails.
   */
  public AprilTagFieldLayout(String path) throws IOException {
    this(Path.of(path));
  }

  /**
   * Construct a new AprilTagFieldLayout with values imported from a JSON file.
   *
   * @param path path of the JSON file to import from
   * @throws IOException if reading from the file fails.
   */
  public AprilTagFieldLayout(Path path) throws IOException {
    this(new ObjectMapper().readValue(path.toFile(), AprilTagFieldLayout.class).m_apriltags);
  }

  /**
   * Construct a new AprilTagFieldLayout from a list of {@link AprilTag} objects.
   *
   * @param apriltags list of {@link AprilTag}
   */
  @JsonCreator
  public AprilTagFieldLayout(
      @JsonProperty(required = true, value = "tags") List<AprilTag> apriltags) {
    // To ensure the underlying semantics don't change with what kind of list is passed in
    m_apriltags.addAll(apriltags);
  }

  /**
   * Set the alliance that your team is on.
   *
   * <p>This changes the {@link #getTag(int)} method to return the correct pose for your alliance.
   *
   * @param alliance the alliance to mirror poses for
   */
  public void setAlliance(DriverStation.Alliance alliance) {
    m_mirror = alliance == DriverStation.Alliance.Red;
  }

  /**
   * Gets an AprilTag pose by its id.
   *
   * @param id The id of the tag
   * @return The pose corresponding to the id passed in.
   */
  public Pose3d getTag(int id) {
    Pose3d pose = m_apriltags.stream().filter(it -> id == it.m_id).findFirst().get().m_pose;
    if (m_mirror) {
      pose =
          pose.relativeTo(
              new Pose3d(
                  new Translation3d(Units.feetToMeters(54.0), Units.feetToMeters(27.0), 0.0),
                  new Rotation3d(0.0, 0.0, Units.degreesToRadians(180.0))));
    }

    return pose;
  }

  /**
   * Serializes a AprilTagFieldLayout to a JSON file.
   *
   * @param path The path to write to
   * @throws IOException if writing to the file fails
   */
  public void serialize(String path) throws IOException {
    serialize(Path.of(path));
  }

  /**
   * Serializes a AprilTagFieldLayout to a JSON file.
   *
   * @param path The path to write to
   * @throws IOException if writing to the file fails
   */
  public void serialize(Path path) throws IOException {
    new ObjectMapper().writeValue(path.toFile(), m_apriltags);
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
}
