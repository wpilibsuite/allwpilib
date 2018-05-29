/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.networktables.NetworkTableType;

import java.util.EnumMap;
import java.util.Map;
import java.util.Objects;

public final class EntryBuilder extends BuilderBase {

  /**
   * The default values to use for each network table type. Booleans default to false,
   * numbers default to 0.0, and string and arrays are all empty.
   */
  private static final Map<NetworkTableType, Object> m_defaultEntryValues =
      new EnumMap<>(NetworkTableType.class);

  static {
    m_defaultEntryValues.put(NetworkTableType.kBoolean, false);
    m_defaultEntryValues.put(NetworkTableType.kDouble, 0.0);
    m_defaultEntryValues.put(NetworkTableType.kString, "");
    m_defaultEntryValues.put(NetworkTableType.kRaw, new byte[0]);
    m_defaultEntryValues.put(NetworkTableType.kBooleanArray, new boolean[0]);
    m_defaultEntryValues.put(NetworkTableType.kDoubleArray, new double[0]);
    m_defaultEntryValues.put(NetworkTableType.kStringArray, new String[0]);
  }

  private final NetworkTableType m_type;

  EntryBuilder(String name, NetworkTableType type) {
    super(name);
    m_type = type;
  }

  @Override
  public TabBuilder toTab(String tabName) {
    return new TabBuilder(tabName);
  }

  private NetworkTableEntry generateEntry() {
    NetworkTableEntry entry = Shuffleboard.getNtInstance().getEntry(generateKey());
    if (!entry.setDefaultValue(m_defaultEntryValues.get(m_type))) {
      entry.forceSetValue(m_defaultEntryValues.get(m_type));
    }
    return entry;
  }

  public final class TabBuilder extends Tab implements TerminalEntryBuilder {

    TabBuilder(String tabName) {
      super(tabName);
      setTab(this);
    }

    @Override
    public LayoutBuilder toLayout(String layoutType, String layoutTitle) {
      Objects.requireNonNull(layoutType, "Layout type must be set");
      Objects.requireNonNull(layoutTitle, "Layout title must be set");
      return new LayoutBuilder(layoutType, layoutTitle);
    }

    @Override
    public WidgetBuilder withWidget(String widgetType) {
      Objects.requireNonNull(widgetType, "Widget type must be set");
      return new WidgetBuilder(widgetType);
    }

    @Override
    public NetworkTableEntry getEntry() {
      return generateEntry();
    }
  }

  public final class WidgetBuilder extends Widget implements TerminalEntryBuilder {

    WidgetBuilder(String widgetType) {
      super(widgetType);
      setWidget(this);
    }

    @Override
    public WidgetBuilder withProperties(Map<String, Object> properties) {
      super.withProperties(properties);
      return this;
    }

    @Override
    public NetworkTableEntry getEntry() {
      return generateEntry();
    }
  }

  public final class LayoutBuilder extends Layout implements TerminalEntryBuilder {

    LayoutBuilder(String layoutType, String layoutTitle) {
      super(layoutType, layoutTitle);
      setLayout(this);
    }

    @Override
    public LayoutBuilder withProperties(Map<String, Object> properties) {
      super.withProperties(properties);
      return this;
    }

    @Override
    public NetworkTableEntry getEntry() {
      return generateEntry();
    }

    @Override
    public WidgetBuilder withWidget(String widgetType) {
      return new WidgetBuilder(widgetType);
    }
  }
}
