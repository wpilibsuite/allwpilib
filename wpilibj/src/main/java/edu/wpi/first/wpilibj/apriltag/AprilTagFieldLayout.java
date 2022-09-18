// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.apriltag;

import edu.wpi.first.math.WPIMathJNI;
import edu.wpi.first.math.geometry.Pose3d;
import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.geometry.Translation3d;
import edu.wpi.first.math.util.Units;
import edu.wpi.first.wpilibj.DriverStation;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.HashMap;
import java.util.Map;

public class AprilTagFieldLayout {
  private final Map<Integer, Pose3d> m_tags = new HashMap<>();
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
    this(
        AprilTagUtil.createAprilTagFieldLayoutFromElements(
            WPIMathJNI.deserializeAprilTagLayout(Files.readString(path))));
  }

  /**
   * Construct a new AprilTagFieldLayout from a map of AprilTag IDs to poses.
   *
   * @param tags map of IDs to poses
   */
  public AprilTagFieldLayout(Map<Integer, Pose3d> tags) {
    // To ensure the underlying semantics don't change with what kind of map is passed in
    m_tags.putAll(tags);
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
   * Gets the raw tags. This is not affected by {@link #setAlliance(DriverStation.Alliance)}.
   *
   * @return map of IDs to poses.
   */
  public Map<Integer, Pose3d> getTags() {
    return m_tags;
  }

  /**
   * Gets an AprilTag pose by its id.
   *
   * @param id The id of the tag
   * @return The pose corresponding to the id passed in.
   */
  public Pose3d getTag(int id) {
    Pose3d tag = m_tags.get(id);
    if (m_mirror) {
      tag =
          tag.relativeTo(
              new Pose3d(
                  new Translation3d(Units.feetToMeters(54.0), Units.feetToMeters(27.0), 0.0),
                  new Rotation3d(0.0, 0.0, 180.0)));
    }

    return tag;
  }

  /**
   * Serialize this layout into a JSON string.
   *
   * @return the serialized JSON string.
   */
  public String serialize() {
    return WPIMathJNI.serializeAprilTagLayout(
        AprilTagUtil.getElementsFromAprilTagFieldLayout(m_tags));
  }
}
