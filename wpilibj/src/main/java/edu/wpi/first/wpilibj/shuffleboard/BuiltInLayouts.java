/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

public enum BuiltInLayouts implements LayoutType {
  kList("List Layout"),
  kGrid("Grid Layout"),
  ;

  private final String layoutName;

  BuiltInLayouts(String layoutName) {
    this.layoutName = layoutName;
  }

  @Override
  public String getLayoutName() {
    return layoutName;
  }
}
