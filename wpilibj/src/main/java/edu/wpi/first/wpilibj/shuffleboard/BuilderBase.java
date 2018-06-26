/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

abstract class BuilderBase {
  private final String m_name;
  private Tab m_tab;
  private Layout m_layout;
  private Widget m_widget;

  BuilderBase(String name) {
    m_name = name;
  }

  public final String getName() {
    return m_name;
  }

  public final Tab getTab() {
    return m_tab;
  }

  final void setTab(Tab tab) {
    m_tab = tab;
  }

  public final Layout getLayout() {
    return m_layout;
  }

  final void setLayout(Layout layout) {
    m_layout = layout;
  }

  public final Widget getWidget() {
    return m_widget;
  }

  final void setWidget(Widget widget) {
    m_widget = widget;
  }

  public abstract Tab toTab(String tabName);

  final String generateKey() {
    String tab = Shuffleboard.BASE_TABLE_NAME + "/" + m_tab.getName() + "/";
    if (m_layout != null) {
      return tab + m_layout.getTitle() + "/" + getName();
    } else {
      return tab + getName();
    }
  }

}
