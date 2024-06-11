// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.shuffleboard;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.networktables.PubSubOption;
import edu.wpi.first.networktables.StringPublisher;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.function.Consumer;

final class ShuffleboardInstance implements ShuffleboardRoot {
  private final Map<String, ShuffleboardTab> m_tabs = new LinkedHashMap<>();
  private boolean m_reported = false; // NOPMD redundant field initializer
  private boolean m_tabsChanged = false; // NOPMD redundant field initializer
  private final NetworkTable m_rootTable;
  private final NetworkTable m_rootMetaTable;
  private final StringPublisher m_selectedTabPub;

  /**
   * Creates a new Shuffleboard instance.
   *
   * @param ntInstance the NetworkTables instance to use
   */
  ShuffleboardInstance(NetworkTableInstance ntInstance) {
    requireNonNullParam(ntInstance, "ntInstance", "ShuffleboardInstance");
    m_rootTable = ntInstance.getTable(Shuffleboard.kBaseTableName);
    m_rootMetaTable = m_rootTable.getSubTable(".metadata");
    m_selectedTabPub =
        m_rootMetaTable.getStringTopic("Selected").publish(PubSubOption.keepDuplicates(true));
  }

  @Override
  public ShuffleboardTab getTab(String title) {
    requireNonNullParam(title, "title", "getTab");
    if (!m_reported) {
      HAL.report(tResourceType.kResourceType_Shuffleboard, 0);
      m_reported = true;
    }
    if (!m_tabs.containsKey(title)) {
      m_tabs.put(title, new ShuffleboardTab(this, title));
      m_tabsChanged = true;
    }
    return m_tabs.get(title);
  }

  @Override
  public void update() {
    if (m_tabsChanged) {
      String[] tabTitles =
          m_tabs.values().stream().map(ShuffleboardTab::getTitle).toArray(String[]::new);
      m_rootMetaTable.getEntry("Tabs").setStringArray(tabTitles);
      m_tabsChanged = false;
    }
    for (ShuffleboardTab tab : m_tabs.values()) {
      String title = tab.getTitle();
      tab.buildInto(m_rootTable, m_rootMetaTable.getSubTable(title));
    }
  }

  @Override
  public void enableActuatorWidgets() {
    applyToAllComplexWidgets(ComplexWidget::enableIfActuator);
  }

  @Override
  public void disableActuatorWidgets() {
    applyToAllComplexWidgets(ComplexWidget::disableIfActuator);
  }

  @Override
  public void selectTab(int index) {
    selectTab(Integer.toString(index));
  }

  @Override
  public void selectTab(String title) {
    m_selectedTabPub.set(title);
  }

  /**
   * Applies the function {@code func} to all complex widgets in this root, regardless of how they
   * are nested.
   *
   * @param func the function to apply to all complex widgets
   */
  private void applyToAllComplexWidgets(Consumer<ComplexWidget> func) {
    for (ShuffleboardTab tab : m_tabs.values()) {
      apply(tab, func);
    }
  }

  /**
   * Applies the function {@code func} to all complex widgets in {@code container}. Helper method
   * for {@link #applyToAllComplexWidgets}.
   */
  private void apply(ShuffleboardContainer container, Consumer<ComplexWidget> func) {
    for (ShuffleboardComponent<?> component : container.getComponents()) {
      if (component instanceof ComplexWidget widget) {
        func.accept(widget);
      }
      if (component instanceof ShuffleboardContainer nestedContainer) {
        apply(nestedContainer, func);
      }
    }
  }
}
