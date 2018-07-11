/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

public interface ShuffleboardRoot {

  /**
   * Gets the tab with the given title, creating it if it does not already exist.
   *
   * @param title the title of the tab
   * @return the tab with the given title
   */
  ShuffleboardTab getTab(String title);

  /**
   * Updates all tabs.
   */
  void update();

}
