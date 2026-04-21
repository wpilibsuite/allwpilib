// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.fields;

public enum Fields {
  FRC_2018_POWER_UP("frc/2018-powerup.json"),
  FRC_2019_DEEP_SPACE("frc/2019-deepspace.json"),
  FRC_2020_INFINITE_RECHARGE("frc/2020-infiniterecharge.json"),
  FRC_2021_INFINITE_RECHARGE("frc/2021-infiniterecharge.json"),
  FRC_2021_BARREL("frc/2021-barrelracingpath.json"),
  FRC_2021_BOUNCE("frc/2021-bouncepath.json"),
  FRC_2021_GALACTIC_SEARCH_A("frc/2021-galacticsearcha.json"),
  FRC_2021_GALACTIC_SEARCH_B("frc/2021-galacticsearchb.json"),
  FRC_2021_SLALOM("frc/2021-slalompath.json"),
  FRC_2022_RAPID_REACT("frc/2022-rapidreact.json"),
  FRC_2023_CHARGED_UP("frc/2023-chargedup.json"),
  FRC_2024_CRESCENDO("frc/2024-crescendo.json"),
  FTC_2024_2025_INTO_THE_DEEP("ftc/2024-2025-intothedeep.json"),
  FRC_2025_REEFSCAPE("frc/2025-reefscape.json"),
  FTC_2025_2026_DECODE("ftc/2025-2026-decode.json"),
  FRC_2026_REBUILT("frc/2026-rebuilt.json");

  public static final String BASE_RESOURCE_DIR = "/org/wpilib/fields/";

  @SuppressWarnings("MemberName")
  public final String resourceFile;

  Fields(String resourceFile) {
    this.resourceFile = BASE_RESOURCE_DIR + resourceFile;
  }
}
