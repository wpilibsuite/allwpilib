/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

public abstract class Tab implements LayoutAdder, WidgetAdder {

  private final String m_name;

  Tab(String name) {
    m_name = name;
  }

  public final String getName() {
    return m_name;
  }

}
