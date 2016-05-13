/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.smartdashboard;

import java.util.ArrayList;

import edu.wpi.first.wpilibj.Sendable;
import edu.wpi.first.wpilibj.command.Command;
import edu.wpi.first.wpilibj.tables.ITable;

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
 * @author Joe Grinstead
 */
public class SendableChooser implements Sendable {

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
   * A table linking strings to the objects the represent.
   */
  private final ArrayList<String> m_choices = new ArrayList<>();
  private final ArrayList<Object> m_values = new ArrayList<>();
  private String m_defaultChoice = null;
  private Object m_defaultValue = null;

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
  public void addObject(String name, Object object) {
    // if we don't have a default, set the default automatically
    if (m_defaultChoice == null) {
      addDefault(name, object);
      return;
    }
    for (int i = 0; i < m_choices.size(); ++i) {
      if (m_choices.get(i).equals(name)) {
        m_choices.set(i, name);
        m_values.set(i, object);
        return;
      }
    }
    // not found
    m_choices.add(name);
    m_values.add(object);

    if (m_table != null) {
      m_table.putStringArray(OPTIONS, m_choices.toArray(new String[0]));
    }
  }

  /**
   * Add the given object to the list of options and marks it as the default. Functionally, this is
   * very close to {@link SendableChooser#addObject(java.lang.String, java.lang.Object)
   * addObject(...)} except that it will use this as the default option if none other is explicitly
   * selected.
   *
   * @param name   the name of the option
   * @param object the option
   */
  public void addDefault(String name, Object object) {
    if (name == null) {
      throw new NullPointerException("Name cannot be null");
    }
    m_defaultChoice = name;
    m_defaultValue = object;
    if (m_table != null) {
      m_table.putString(DEFAULT, m_defaultChoice);
    }
    addObject(name, object);
  }

  /**
   * Returns the selected option. If there is none selected, it will return the default. If there is
   * none selected and no default, then it will return {@code null}.
   *
   * @return the option selected
   */
  public Object getSelected() {
    String selected = m_table.getString(SELECTED, null);
    for (int i = 0; i < m_values.size(); ++i) {
      if (m_choices.get(i).equals(selected)) {
        return m_values.get(i);
      }
    }
    return m_defaultValue;

  }

  @Override
  public String getSmartDashboardType() {
    return "String Chooser";
  }

  private ITable m_table;

  @Override
  public void initTable(ITable table) {
    m_table = table;
    if (table != null) {
      table.putStringArray(OPTIONS, m_choices.toArray(new String[0]));
      if (m_defaultChoice != null) {
        table.putString(DEFAULT, m_defaultChoice);
      }
    }
  }

  @Override
  public ITable getTable() {
    return m_table;
  }
}
