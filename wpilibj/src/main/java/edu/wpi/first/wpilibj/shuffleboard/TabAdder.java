/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

// Used to make sure the various builders have a consistent interface
public interface TabAdder {

  /**
   * Sets the tab in Shuffleboard in which this data should be displayed. If no tab exists with the
   * given name, it will be created.
   *
   * @param tabName the name of the tab
   */
  Tab toTab(String tabName);

}
