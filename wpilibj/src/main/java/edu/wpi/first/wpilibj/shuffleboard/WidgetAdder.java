/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

// Used to make sure the various builders have a consistent interface
public interface WidgetAdder {

  /**
   * Sets the widget with which Shuffleboard should display the data. This should be used when you
   * want to use a different widget than the default one that Shuffleboard uses, or if you want to
   * set the values of certain properties on the default widget.
   *
   * <p>If no widget with the given type is available in Shuffleboard, the default widget will be
   * used. Any properties set will be ignored.</p>
   *
   * @param widgetType the type of the widget with which to display this data
   */
  Widget withWidget(String widgetType);

}
