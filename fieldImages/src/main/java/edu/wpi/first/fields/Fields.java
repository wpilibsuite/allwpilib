// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.fields;

public enum Fields {
  POWER_UP_2018("2018-powerup.json"),
  DEEP_SPACE_2019("2019-deepspace.json"),
  INFINITE_RECHARGE_2020("2020-infiniterecharge.json"),
  INFINITE_RECHARGE_2021("2021-infiniterecharge.json"),
  BARREL_2021("2021-barrelracingpath.json"),
  BOUNCE_2021("2021-bouncepath.json"),
  GALACTIC_SEARCHA_2021("2021-galacticsearcha.json"),
  GALACTIC_SEARCHB_2021("2021-galacticsearchb.json"),
  SLALOM_2021("2021-slalompath.json"),
  RAPID_REACT_2022("2022-rapidreact.json"),
  CHARGED_UP_2023("2023-chargedup.json"),
  CRESCENDO_2024("2024-crescendo.json"),
  REEFSCAPE_2025("2025-reefscape.json");

  public static final String BASE_RESOURCE_DIR = "/edu/wpi/first/fields/";

  /** Alias to the current game. */
  public static final Fields DEFAULT_FIELD = REEFSCAPE_2025;

  public final String m_resourceFile;

  Fields(String resourceFile) {
    m_resourceFile = BASE_RESOURCE_DIR + resourceFile;
  }
}
