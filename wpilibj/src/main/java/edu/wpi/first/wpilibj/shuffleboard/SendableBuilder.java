/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

import edu.wpi.first.wpilibj.Sendable;

public final class SendableBuilder extends BuilderBase {

  private final Sendable m_sendable;

  SendableBuilder(String sendableName, Sendable sendable) {
    super(sendableName);
    m_sendable = sendable;
  }

  @Override
  public Tab toTab(String tabName) {
    return new TabBuilder(tabName);
  }

  Sendable getSendable() {
    return m_sendable;
  }

  private final class TabBuilder extends Tab {

    TabBuilder(String tabName) {
      super(tabName);
      setTab(this);
    }

    @Override
    public Layout toLayout(String layoutType, String layoutTitle) {
      return new LayoutBuilder(layoutType, layoutTitle);
    }

    @Override
    public Widget withWidget(String widgetType) {
      return new WidgetBuilder(widgetType);
    }
  }

  public final class LayoutBuilder extends Layout {

    LayoutBuilder(String layoutType, String layoutTitle) {
      super(layoutType, layoutTitle);
      setLayout(this);
    }

    @Override
    public Widget withWidget(String widgetType) {
      return new WidgetBuilder(widgetType);
    }

  }

  private final class WidgetBuilder extends Widget {

    WidgetBuilder(String widgetType) {
      super(widgetType);
      setWidget(this);
    }

  }

}
