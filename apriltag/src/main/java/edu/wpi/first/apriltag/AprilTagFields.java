// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.apriltag;

public enum AprilTagFields {
  k2022RapidReact("2022-rapidreact.json"),
  k2023ChargedUp("2023-chargedup.json");

  public static final String kBaseResourceDir = "/edu/wpi/first/apriltag/";

  /** Alias to the current game. */
  public static final AprilTagFields kDefaultField = k2023ChargedUp;

  public final String m_resourceFile;

  AprilTagFields(String resourceFile) {
    m_resourceFile = kBaseResourceDir + resourceFile;
  }
}
