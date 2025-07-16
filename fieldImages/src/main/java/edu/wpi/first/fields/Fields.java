// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.fields;

public enum Fields {
  // TODO(crueter): Naming could be better
  k2018FRCPowerUp("2018-frc-powerup.json"),
  k2019FRCDeepSpace("2019-frc-deepspace.json"),
  k2020FRCInfiniteRecharge("2020-frc-infiniterecharge.json"),
  k2021FRCInfiniteRecharge("2021-frc-infiniterecharge.json"),
  k2021FRCBarrel("2021-frc-barrelracingpath.json"),
  k2021FRCBounce("2021-frc-bouncepath.json"),
  k2021FRCGalacticSearchA("2021-frc-galacticsearcha.json"),
  k2021FRCGalacticSearchB("2021-frc-galacticsearchb.json"),
  k2021FRCSlalom("2021-frc-slalompath.json"),
  k2022FRCRapidReact("2022-frc-rapidreact.json"),
  k2023FRCChargedUp("2023-frc-chargedup.json"),
  k2024FRCCrescendo("2024-frc-crescendo.json"),
  k20242025FTCIntoTheDeep("2024-2025-ftc-intothedeep.json"),
  k2025FRCReefscape("2025-frc-reefscape.json");

  public static final String kBaseResourceDir = "/edu/wpi/first/fields/";

  /** Alias to the current game. */
  public static final Fields kDefaultField = k2025FRCReefscape;

  public final String m_resourceFile;

  Fields(String resourceFile) {
    m_resourceFile = kBaseResourceDir + resourceFile;
  }
}
