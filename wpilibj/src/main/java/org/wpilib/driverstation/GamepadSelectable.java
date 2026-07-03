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
 * <p>The D-pad up and down buttons select which chooser is active. The D-pad left and right buttons
 * change the selected option in the active chooser.
 */
public class GamepadSelectable {
  private static final AtomicInteger s_instances = new AtomicInteger();

  private final Gamepad m_gamepad;
  private final List<Chooser> m_choosers = new ArrayList<>();
  private final Map<String, Chooser> m_chooserMap = new LinkedHashMap<>();
  private final String m_captionPrefix;
  private int m_selectedChooser;

  /**
   * Constructs a GamepadSelectable.
   *
   * @param port The port index on the Driver Station that the gamepad is plugged into (0-5).
   */
  public GamepadSelectable(int port) {
    this(DriverStation.getGamepad(port));
  }

  /**
   * Constructs a GamepadSelectable.
   *
   * @param gamepad The gamepad used to change selections.
   */
  public GamepadSelectable(Gamepad gamepad) {
    m_gamepad = requireNonNullParam(gamepad, "gamepad", "GamepadSelectable");
    m_captionPrefix = "GamepadSelectable/" + s_instances.getAndIncrement() + "/";
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
   * Adds a chooser with the given options.
   *
   * <p>The first option is selected by default.
   *
   * @param name chooser name
   * @param options available options
   * @return the created chooser
   */
  public Chooser addOptions(String name, List<String> options) {
    requireChooserName(name, "addOptions");
    requireNonNullParam(options, "options", "addOptions");
    if (options.isEmpty()) {
      throw new IllegalArgumentException("Options cannot be empty");
    }
    if (m_chooserMap.containsKey(name)) {
      throw new IllegalArgumentException("Chooser already exists: " + name);
    }

    var chooser = new Chooser(name, options);
    m_choosers.add(chooser);
    m_chooserMap.put(name, chooser);
    return chooser;
  }

  /**
   * Adds a chooser with integer options from {@code min} to {@code max}, inclusive, stepping by
   * {@code delta}.
   *
   * <p>The first option is selected by default.
   *
   * @param name chooser name
   * @param min minimum option
   * @param max maximum option
   * @param delta amount between options
   * @return the created chooser
   */
  public Chooser addIntegerOptions(String name, int min, int max, int delta) {
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
   * Adds a chooser with floating point options from {@code min} to {@code max}, inclusive, stepping
   * by {@code delta}.
   *
   * <p>The first option is selected by default.
   *
   * @param name chooser name
   * @param min minimum option
   * @param max maximum option
   * @param delta amount between options
   * @return the created chooser
   */
  public Chooser addDoubleOptions(String name, double min, double max, double delta) {
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
    while (value.compareTo(maximum) <= 0) {
      options.add(value.toPlainString());
      value = value.add(increment);
    }
    String maximumString = maximum.toPlainString();
    if (!options.get(options.size() - 1).equals(maximumString)) {
      options.add(maximumString);
    }
    return addOptions(name, options);
  }

  /**
   * Updates chooser selection and adds the current selections to the driver station display.
   *
   * <p>The D-pad up and down buttons select which chooser is active. The D-pad left and right
   * buttons change the selected option in the active chooser.
   *
   * <p>Call {@link DriverStationDisplay#updateLines()} externally to flush display updates.
   */
  public void update() {
    if (m_choosers.isEmpty()) {
      return;
    }

    if (m_gamepad.getDpadUpButtonPressed()) {
      m_selectedChooser = wrap(m_selectedChooser - 1, m_choosers.size());
    }
    if (m_gamepad.getDpadDownButtonPressed()) {
      m_selectedChooser = wrap(m_selectedChooser + 1, m_choosers.size());
    }

    Chooser chooser = m_choosers.get(m_selectedChooser);
    if (m_gamepad.getDpadLeftButtonPressed()) {
      chooser.selectPrevious();
    }
    if (m_gamepad.getDpadRightButtonPressed()) {
      chooser.selectNext();
    }

    for (int i = 0; i < m_choosers.size(); i++) {
      Chooser displayChooser = m_choosers.get(i);
      DriverStationDisplay.addData(
          m_captionPrefix + displayChooser.getName(),
          formatDisplayLine(displayChooser, i == m_selectedChooser));
    }
  }

  /**
   * Gets the currently selected option for a chooser.
   *
   * @param name chooser name
   * @return the currently selected option
   */
  public String getSelected(String name) {
    return getChooser(name).getSelected();
  }

  /**
   * Gets the currently selected integer option for a chooser.
   *
   * @param name chooser name
   * @return the currently selected option
   */
  public int getSelectedInteger(String name) {
    return Integer.parseInt(getSelected(name));
  }

  /**
   * Gets the currently selected double option for a chooser.
   *
   * @param name chooser name
   * @return the currently selected option
   */
  public double getSelectedDouble(String name) {
    return Double.parseDouble(getSelected(name));
  }

  /**
   * Gets the currently selected option index for a chooser.
   *
   * @param name chooser name
   * @return the currently selected option index
   */
  public int getSelectedIndex(String name) {
    return getChooser(name).getSelectedIndex();
  }

  /**
   * Gets the names of all choosers.
   *
   * @return chooser names
   */
  public List<String> getChooserNames() {
    return List.copyOf(m_chooserMap.keySet());
  }

  /**
   * Gets the chooser currently controlled by D-pad left and right.
   *
   * @return the selected chooser
   */
  public Chooser getSelectedChooser() {
    if (m_choosers.isEmpty()) {
      return null;
    }
    return m_choosers.get(m_selectedChooser);
  }

  private Chooser getChooser(String name) {
    requireChooserName(name, "getChooser");
    Chooser chooser = m_chooserMap.get(name);
    if (chooser == null) {
      throw new IllegalArgumentException("Unknown chooser: " + name);
    }
    return chooser;
  }

  private static void requireChooserName(String name, String methodName) {
    requireNonNullParam(name, "name", methodName);
    if (name.isBlank()) {
      throw new IllegalArgumentException("Chooser name cannot be blank");
    }
  }

  private static int wrap(int value, int size) {
    return Math.floorMod(value, size);
  }

  private static String formatDisplayLine(Chooser chooser, boolean selected) {
    if (selected) {
      return "> " + chooser.getName() + ": " + chooser.getSelected() + " <";
    }
    return chooser.getName() + " : " + chooser.getSelected();
  }

  /** A single named set of selectable options. */
  public static final class Chooser {
    private final String m_name;
    private final List<String> m_options;
    private int m_selectedIndex;

    private Chooser(String name, List<String> options) {
      m_name = name;
      m_options = Collections.unmodifiableList(new ArrayList<>(options));
    }

    /**
     * Gets the chooser name.
     *
     * @return chooser name
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
