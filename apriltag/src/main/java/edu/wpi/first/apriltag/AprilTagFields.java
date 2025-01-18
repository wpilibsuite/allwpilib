// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.apriltag;

import java.io.UncheckedIOException;

/** Loadable AprilTag field layouts. */
public enum AprilTagFields {
  /** 2022 Rapid React. */
  k2022RapidReact("2022-rapidreact.json"),
  /** 2023 Charged Up. */
  k2023ChargedUp("2023-chargedup.json"),
  /** 2024 Crescendo. */
  k2024Crescendo("2024-crescendo.json"),
  /** 2025 Reefscape. */
  k2025Reefscape("2025-reefscape.json");

  /** Base resource directory. */
  public static final String kBaseResourceDir = "/edu/wpi/first/apriltag/";

  /** Alias to the current game. */
  public static final AprilTagFields kDefaultField = k2025Reefscape;

  /** Resource filename. */
  public final String m_resourceFile;

  AprilTagFieldLayout m_fieldLayout;

  AprilTagFields(String resourceFile) {
    m_resourceFile = kBaseResourceDir + resourceFile;
  }

  /**
   * Get a {@link AprilTagFieldLayout} from the resource JSON.
   *
   * @return AprilTagFieldLayout of the field
   * @throws UncheckedIOException If the layout does not exist
   * @deprecated Use {@link AprilTagFieldLayout#loadField(AprilTagFields)} instead.
   */
  @Deprecated(forRemoval = true, since = "2025")
  public AprilTagFieldLayout loadAprilTagLayoutField() {
    return AprilTagFieldLayout.loadField(this);
  }
}
