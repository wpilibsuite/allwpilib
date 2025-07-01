// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.fields;

public class Fields {
  public static final String kBaseResourceDir = "/edu/wpi/first/fields/";

  public enum FRC implements IField {
    k2018PowerUp("2018-powerup.json"),
    k2019DeepSpace("2019-deepspace.json"),
    k2020InfiniteRecharge("2020-infiniterecharge.json"),
    k2021InfiniteRecharge("2021-infiniterecharge.json"),
    k2021Barrel("2021-barrelracingpath.json"),
    k2021Bounce("2021-bouncepath.json"),
    k2021GalacticSearchA("2021-galacticsearcha.json"),
    k2021GalacticSearchB("2021-galacticsearchb.json"),
    k2021Slalom("2021-slalompath.json"),
    k2022RapidReact("2022-rapidreact.json"),
    k2023ChargedUp("2023-chargedup.json"),
    k2024Crescendo("2024-crescendo.json"),
    k2025Reefscape("2025-reefscape.json");

    public final String m_resourceFile;

    @Override
    public String getResourceFile() {
      return m_resourceFile;
    }

    FRC(String resourceFile) {
      m_resourceFile = kBaseResourceDir + "frc/" + resourceFile;
    }
  }

  public enum FTC implements IField {
    k20242025IntoTheDeep("2024-2025-intothedeep.json");

    public final String m_resourceFile;

    @Override
    public String getResourceFile() {
      return m_resourceFile;
    }

    FTC(String resourceFile) {
      m_resourceFile = kBaseResourceDir + "ftc/" + resourceFile;
    }
  }
}
