/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

// Used to make sure the various builders have a consistent interface
public interface LayoutAdder {

  /**
   * Specifies a layout in which the data should be displayed. This is <i>optional</i>; if set,
   * the data will be placed inside a matching layout. If no layout is available in Shuffleboard
   * with the given type name, the widget will be placed inside the tab. If a layout already exists
   * with the given title and type, the widget will be placed inside it; otherwise, a new layout
   * will be created to contain the widget.
   *
   * <p>If no layout with the given type is available in Shuffleboard, then the resulting widget
   * will be placed in the tab root.</p>
   *
   * @param layoutType  the type of the layout in which to place the widget
   * @param layoutTitle the title of the layout
   */
  Layout toLayout(String layoutType, String layoutTitle);

}
