// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.shuffleboard;

/**
 * The importance of an event marker in Shuffleboard. The exact meaning of each importance level is
 * up for interpretation on a team-to-team basis, but users should follow the general guidelines of
 * the various importance levels. The examples given are for reference and may be ignored or
 * considered to be more or less important from team to team.
 */
public enum EventImportance {
  // Maintainer note: this enum is mirrored in WPILibC and in Shuffleboard
  // Modifying the enum or enum strings requires a corresponding change to the C++ enum
  // and the enum in Shuffleboard

  /** A trivial event such as a change in command state. */
  kTrivial("TRIVIAL"),

  /** A low importance event such as acquisition of a game piece. */
  kLow("LOW"),

  /**
   * A "normal" importance event, such as a transition from autonomous mode to teleoperated control.
   */
  kNormal("NORMAL"),

  /** A high-importance event such as scoring a game piece. */
  kHigh("HIGH"),

  /** A critically important event such as a brownout, component failure, or software deadlock. */
  kCritical("CRITICAL");

  private final String m_simpleName;

  EventImportance(String simpleName) {
    m_simpleName = simpleName;
  }

  public String getSimpleName() {
    return m_simpleName;
  }
}
