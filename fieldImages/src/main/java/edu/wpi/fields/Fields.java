// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.fields;

@SuppressWarnings("deprecation")
public enum Fields {
  k2018PowerUp(FieldImages.k2018PowerUpFieldConfig),
  k2019DeepSpace(FieldImages.k2019DeepSpaceFieldConfig),
  k2020InfiniteRecharge(FieldImages.k2020InfiniteRechargeFieldConfig),
  k2021InfiniteRecharge(FieldImages.k2021InfiniteRechargeFieldConfig),
  k2021Barrel(FieldImages.k2021BarrelFieldConfig),
  k2021Bounce(FieldImages.k2021BounceFieldConfig),
  k2021GalacticSearchA(FieldImages.k2021GalacticSearchAFieldConfig),
  k2021GalacticSearchB(FieldImages.k2021GalacticSearchBFieldConfig),
  k2021Slalom(FieldImages.k2021SlalomFieldConfig),
  k2022RapidReact(FieldImages.k2022RapidReactFieldConfig);

  /** Alias to the current game. */
  public static final Fields kDefaultField = k2022RapidReact;

  public final String m_resourceFile;

  Fields(String resourceFile) {
    m_resourceFile = resourceFile;
  }
}
