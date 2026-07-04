// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.driverstation;

import static org.wpilib.util.ErrorMessages.requireNonNullParam;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.Collections;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * A set of selectable options controlled by a {@link Gamepad}.
 *
 * <p>The D-pad up and down buttons select which selectable is active. The D-pad left and right
 * buttons change the selected option in the active selectable.
 */
public class DSGamepadChooser {
  private static final AtomicInteger s_instances = new AtomicInteger();

  private final Gamepad m_gamepad;
  private final List<GamepadSelectable> m_selectables = new ArrayList<>();
  private final Map<String, GamepadSelectable> m_selectableMap = new LinkedHashMap<>();
  private final String m_captionPrefix;
  private int m_selectedSelectable;

  /**
   * Constructs a DSGamepadChooser.
   *
   * @param port The port index on the Driver Station that the gamepad is plugged into (0-5).
   */
  public DSGamepadChooser(int port) {
    this(DriverStation.getGamepad(port));
  }

  /**
   * Constructs a DSGamepadChooser.
   *
   * @param gamepad The gamepad used to change selections.
   */
  public DSGamepadChooser(Gamepad gamepad) {
    m_gamepad = requireNonNullParam(gamepad, "gamepad", "DSGamepadChooser");
    m_captionPrefix = "DSGamepadChooser/" + s_instances.getAndIncrement() + "/";
  }

  /**
   * Get the gamepad used to change selections.
   *
   * @return the gamepad
   */
  public Gamepad getGamepad() {
    return m_gamepad;
  }

  /**
   * Adds a selectable with the given options.
   *
   * <p>The first option is selected by default.
   *
   * @param name selectable name
   * @param options available options
   * @return the created selectable
   */
  public GamepadSelectable addOptions(String name, List<String> options) {
    requireSelectableName(name, "addOptions");
    requireNonNullParam(options, "options", "addOptions");
    if (options.isEmpty()) {
      throw new IllegalArgumentException("Options cannot be empty");
    }
    if (m_selectableMap.containsKey(name)) {
      throw new IllegalArgumentException("GamepadSelectable already exists: " + name);
    }

    var selectable = new GamepadSelectable(name, options);
    m_selectables.add(selectable);
    m_selectableMap.put(name, selectable);
    return selectable;
  }

  /**
   * Adds a selectable with integer options from {@code min} to {@code max}, inclusive, stepping by
   * {@code delta}.
   *
   * <p>The first option is selected by default.
   *
   * @param name selectable name
   * @param min minimum option
   * @param max maximum option
   * @param delta amount between options
   * @return the created selectable
   */
  public GamepadSelectable addIntegerOptions(String name, int min, int max, int delta) {
    if (min > max) {
      throw new IllegalArgumentException("Minimum cannot be greater than maximum");
    }
    if (delta <= 0) {
      throw new IllegalArgumentException("Delta must be greater than zero");
    }

    List<String> options = new ArrayList<>();
    for (long value = min; value <= max; value += delta) {
      options.add(String.valueOf(value));
    }
    if (!options.get(options.size() - 1).equals(String.valueOf(max))) {
      options.add(String.valueOf(max));
    }
    return addOptions(name, options);
  }

  /**
   * Adds a selectable with floating point options from {@code min} to {@code max}, inclusive,
   * stepping by {@code delta}.
   *
   * <p>The first option is selected by default.
   *
   * @param name selectable name
   * @param min minimum option
   * @param max maximum option
   * @param delta amount between options
   * @return the created selectable
   */
  public GamepadSelectable addDoubleOptions(String name, double min, double max, double delta) {
    if (!Double.isFinite(min) || !Double.isFinite(max) || !Double.isFinite(delta)) {
      throw new IllegalArgumentException("Minimum, maximum, and delta must be finite");
    }
    if (min > max) {
      throw new IllegalArgumentException("Minimum cannot be greater than maximum");
    }
    if (delta <= 0.0) {
      throw new IllegalArgumentException("Delta must be greater than zero");
    }

    BigDecimal value = BigDecimal.valueOf(min);
    BigDecimal maximum = BigDecimal.valueOf(max);
    BigDecimal increment = BigDecimal.valueOf(delta);
    List<String> options = new ArrayList<>();
    BigDecimal lastValue = value;
    while (value.compareTo(maximum) <= 0) {
      options.add(value.toPlainString());
      lastValue = value;
      value = value.add(increment);
    }
    String maximumString = maximum.toPlainString();
    if (lastValue.compareTo(maximum) != 0) {
      options.add(maximumString);
    }
    return addOptions(name, options);
  }

  /**
   * Updates selectable selection and adds the current selections to the driver station display.
   *
   * <p>The D-pad up and down buttons select which selectable is active. The D-pad left and right
   * buttons change the selected option in the active selectable.
   *
   * <p>Call {@link DriverStationDisplay#updateLines()} externally to flush display updates.
   */
  public void update() {
    if (m_selectables.isEmpty()) {
      return;
    }

    if (m_gamepad.getDpadUpButtonPressed()) {
      m_selectedSelectable = wrap(m_selectedSelectable - 1, m_selectables.size());
    }
    if (m_gamepad.getDpadDownButtonPressed()) {
      m_selectedSelectable = wrap(m_selectedSelectable + 1, m_selectables.size());
    }

    GamepadSelectable selectable = m_selectables.get(m_selectedSelectable);
    if (m_gamepad.getDpadLeftButtonPressed()) {
      selectable.selectPrevious();
    }
    if (m_gamepad.getDpadRightButtonPressed()) {
      selectable.selectNext();
    }

    for (int i = 0; i < m_selectables.size(); i++) {
      GamepadSelectable displaySelectable = m_selectables.get(i);
      DriverStationDisplay.addData(
          m_captionPrefix + displaySelectable.getName(),
          formatDisplayLine(displaySelectable, i == m_selectedSelectable));
    }
  }

  /**
   * Gets the currently selected option for a selectable.
   *
   * @param name selectable name
   * @return the currently selected option
   */
  public String getSelected(String name) {
    return getSelectable(name).getSelected();
  }

  /**
   * Gets the currently selected integer option for a selectable.
   *
   * @param name selectable name
   * @return the currently selected option
   */
  public int getSelectedInteger(String name) {
    return Integer.parseInt(getSelected(name));
  }

  /**
   * Gets the currently selected double option for a selectable.
   *
   * @param name selectable name
   * @return the currently selected option
   */
  public double getSelectedDouble(String name) {
    return Double.parseDouble(getSelected(name));
  }

  /**
   * Gets the currently selected option index for a selectable.
   *
   * @param name selectable name
   * @return the currently selected option index
   */
  public int getSelectedIndex(String name) {
    return getSelectable(name).getSelectedIndex();
  }

  /**
   * Gets the names of all selectables.
   *
   * @return selectable names
   */
  public List<String> getSelectableNames() {
    return List.copyOf(m_selectableMap.keySet());
  }

  /**
   * Gets the selectable currently controlled by D-pad left and right.
   *
   * @return the selected selectable
   */
  public GamepadSelectable getSelectedSelectable() {
    if (m_selectables.isEmpty()) {
      return null;
    }
    return m_selectables.get(m_selectedSelectable);
  }

  private GamepadSelectable getSelectable(String name) {
    requireSelectableName(name, "getSelectable");
    GamepadSelectable selectable = m_selectableMap.get(name);
    if (selectable == null) {
      throw new IllegalArgumentException("Unknown selectable: " + name);
    }
    return selectable;
  }

  private static void requireSelectableName(String name, String methodName) {
    requireNonNullParam(name, "name", methodName);
    if (name.isBlank()) {
      throw new IllegalArgumentException("GamepadSelectable name cannot be blank");
    }
  }

  private static int wrap(int value, int size) {
    return Math.floorMod(value, size);
  }

  private static String formatDisplayLine(GamepadSelectable selectable, boolean selected) {
    if (selected) {
      return "> " + selectable.getName() + " : " + selectable.getSelected() + " <";
    }
    return "  " + selectable.getName() + " : " + selectable.getSelected();
  }

  /** A single named set of selectable options. */
  public static final class GamepadSelectable {
    private final String m_name;
    private final List<String> m_options;
    private int m_selectedIndex;

    private GamepadSelectable(String name, List<String> options) {
      m_name = name;
      m_options = Collections.unmodifiableList(new ArrayList<>(options));
    }

    /**
     * Gets the selectable name.
     *
     * @return selectable name
     */
    public String getName() {
      return m_name;
    }

    /**
     * Gets the available options.
     *
     * @return available options
     */
    public List<String> getOptions() {
      return m_options;
    }

    /**
     * Gets the currently selected option.
     *
     * @return the currently selected option
     */
    public String getSelected() {
      return m_options.get(m_selectedIndex);
    }

    /**
     * Gets the currently selected option index.
     *
     * @return the currently selected option index
     */
    public int getSelectedIndex() {
      return m_selectedIndex;
    }

    private void selectPrevious() {
      m_selectedIndex = wrap(m_selectedIndex - 1, m_options.size());
    }

    private void selectNext() {
      m_selectedIndex = wrap(m_selectedIndex + 1, m_options.size());
    }
  }
}
