/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Objects;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableInstance;

final class ShuffleboardInstance implements ShuffleboardRoot {
  private final Map<String, ShuffleboardTab> m_tabs = new LinkedHashMap<>();

  private boolean m_tabsChanged = false; // NOPMD redundant field initializer
  private final NetworkTable m_rootTable;
  private final NetworkTable m_rootMetaTable;

  ShuffleboardInstance(NetworkTableInstance ntInstance) {
    Objects.requireNonNull(ntInstance, "NetworkTable instance cannot be null");
    m_rootTable = ntInstance.getTable(Shuffleboard.BASE_TABLE_NAME);
    m_rootMetaTable = m_rootTable.getSubTable(".metadata");
  }

  @Override
  public ShuffleboardTab getTab(String title) {
    Objects.requireNonNull(title, "Tab title cannot be null");
    if (!m_tabs.containsKey(title)) {
      m_tabs.put(title, new ShuffleboardTab(this, title));
      m_tabsChanged = true;
    }
    return m_tabs.get(title);
  }

  @Override
  public void update() {
    if (m_tabsChanged) {
      String[] tabTitles = m_tabs.values()
          .stream()
          .map(ShuffleboardTab::getTitle)
          .toArray(String[]::new);
      m_rootMetaTable.getEntry("Tabs").forceSetStringArray(tabTitles);
      m_tabsChanged = false;
    }
    for (ShuffleboardTab tab : m_tabs.values()) {
      String title = tab.getTitle();
      tab.buildInto(m_rootTable, m_rootMetaTable.getSubTable(title));
    }
  }

  @Override
  public void enableActuatorWidgets() {
    for (ShuffleboardTab tab : m_tabs.values()) {
      for (ShuffleboardComponent<?> component : tab.getComponents()) {
        if (component instanceof ComplexWidget) {
          ComplexWidget complexWidget = (ComplexWidget) component;
          complexWidget.enableIfActuator();
        }
      }
    }
  }

  @Override
  public void disableActuatorWidgets() {
    for (ShuffleboardTab tab : m_tabs.values()) {
      for (ShuffleboardComponent<?> component : tab.getComponents()) {
        if (component instanceof ComplexWidget) {
          ComplexWidget complexWidget = (ComplexWidget) component;
          complexWidget.disableIfActuator();
        }
      }
    }
  }
}
