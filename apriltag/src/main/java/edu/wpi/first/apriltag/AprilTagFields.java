// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.apriltag;

import java.io.UncheckedIOException;

/** Loadable AprilTag field layouts. */
public enum AprilTagFields {
  /** 2022 Rapid React. */
  RAPID_REACT_2022("2022-rapidreact.json"),
  /** 2023 Charged Up. */
  CHARGED_UP_2023("2023-chargedup.json"),
  /** 2024 Crescendo. */
  CRESCENDO_2024("2024-crescendo.json"),
  /** 2025 Reefscape Welded (see TU 12). */
  REEFSCAPE_2025_WELDED("2025-reefscape-welded.json"),
  /** 2025 Reefscape AndyMark (see TU 12). */
  REEFSCAPE_2025_ANDYMARK("2025-reefscape-andymark.json");

  /** Base resource directory. */
  public static final String BASE_RESOURCE_DIR = "/edu/wpi/first/apriltag/";

  /** Alias to the current game. */
  public static final AprilTagFields DEFAULT_FIELD = REEFSCAPE_2025_WELDED;

  /** Resource filename. */
  public final String m_resourceFile;

  AprilTagFieldLayout m_fieldLayout;

  AprilTagFields(String resourceFile) {
    m_resourceFile = BASE_RESOURCE_DIR + resourceFile;
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
