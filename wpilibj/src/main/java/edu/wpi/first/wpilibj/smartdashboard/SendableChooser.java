/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.smartdashboard;

import java.util.LinkedHashMap;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.wpilibj.Sendable;
import edu.wpi.first.wpilibj.command.Command;

import static java.util.Objects.requireNonNull;

/**
 * The {@link SendableChooser} class is a useful tool for presenting a selection of options to the
 * {@link SmartDashboard}.
 *
 * <p>For instance, you may wish to be able to select between multiple autonomous modes. You can do
 * this by putting every possible {@link Command} you want to run as an autonomous into a {@link
 * SendableChooser} and then put it into the {@link SmartDashboard} to have a list of options appear
 * on the laptop. Once autonomous starts, simply ask the {@link SendableChooser} what the selected
 * value is.
 *
 * @param <V> The type of the values to be stored
 */
public class SendableChooser<V> implements Sendable {

  /**
   * The key for the default value.
   */
  private static final String DEFAULT = "default";
  /**
   * The key for the selected option.
   */
  private static final String SELECTED = "selected";
  /**
   * The key for the option array.
   */
  private static final String OPTIONS = "options";
  /**
   * A map linking strings to the objects the represent.
   */
  private final LinkedHashMap<String, V> m_map = new LinkedHashMap<>();
  private String m_defaultChoice = null;

  /**
   * Instantiates a {@link SendableChooser}.
   */
  public SendableChooser() {
  }

  /**
   * Adds the given object to the list of options. On the {@link SmartDashboard} on the desktop, the
   * object will appear as the given name.
   *
   * @param name   the name of the option
   * @param object the option
   */
  public void addObject(String name, V object) {
    m_map.put(name, object);

    if (m_tableOptions != null) {
      m_tableOptions.setStringArray(m_map.keySet().toArray(new String[0]));
    }
  }

  /**
   * Add the given object to the list of options and marks it as the default. Functionally, this is
   * very close to {@link #addObject(String, Object)} except that it will use this as the default
   * option if none other is explicitly selected.
   *
   * @param name   the name of the option
   * @param object the option
   */
  public void addDefault(String name, V object) {
    requireNonNull(name, "Provided name was null");

    m_defaultChoice = name;
    if (m_tableDefault != null) {
      m_tableDefault.setString(m_defaultChoice);
    }
    addObject(name, object);
  }

  /**
   * Returns the selected option. If there is none selected, it will return the default. If there is
   * none selected and no default, then it will return {@code null}.
   *
   * @return the option selected
   */
  public V getSelected() {
    String selected = m_tableSelected.getString(null);
    return m_map.getOrDefault(selected, m_map.get(m_defaultChoice));
  }

  @Override
  public String getSmartDashboardType() {
    return "String Chooser";
  }

  private NetworkTableEntry m_tableDefault;
  private NetworkTableEntry m_tableSelected;
  private NetworkTableEntry m_tableOptions;

  @Override
  public void initTable(NetworkTable table) {
    if (table != null) {
      m_tableDefault = table.getEntry(DEFAULT);
      m_tableSelected = table.getEntry(SELECTED);
      m_tableOptions = table.getEntry(OPTIONS);
      m_tableOptions.setStringArray(m_map.keySet().toArray(new String[0]));
      if (m_defaultChoice != null) {
        m_tableDefault.setString(m_defaultChoice);
      }
    }
  }
}
