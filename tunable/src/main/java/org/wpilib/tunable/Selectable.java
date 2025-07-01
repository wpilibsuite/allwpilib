// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

import static org.wpilib.util.ErrorMessages.requireNonNullParam;

import java.util.LinkedHashMap;
import java.util.Map;
import java.util.function.Consumer;

/**
 * The {@link Selectable} class is a useful tool for presenting a selection of options as a tunable.
 *
 * @param <V> The type of the values to be stored
 */
public final class Selectable<V> implements ComplexTunable {
  /** The key for the default value. */
  private static final String DEFAULT = "default";

  /** The key for the selected option. */
  private static final String SELECTED = "selected";

  /** The key for the option array. */
  private static final String OPTIONS = "options";

  /** A map linking strings to the objects they represent. */
  private final Map<String, V> m_map = new LinkedHashMap<>();

  private final Tunable<String> m_defaultChoice =
      Tunable.createConfig("", TunableConfig.of(TunableOption.IMMUTABLE));
  private Consumer<V> m_listener;
  private final Tunable<String[]> m_options =
      Tunable.createConfig(new String[0], TunableConfig.of(TunableOption.IMMUTABLE));
  private final Tunable<String> m_selected =
      Tunable.createConfig(
          "",
          TunableConfig.of(
              TunableOption.ROBUST,
              TunableOption.onTune(
                  () -> {
                    if (m_listener != null) {
                      V selected = getSelected();
                      if (selected != null) {
                        m_listener.accept(selected);
                      }
                    }
                  })));

  /**
   * Adds the given object to the list of options.
   *
   * @param name the name of the option
   * @param object the option
   */
  public void add(String name, V object) {
    m_map.put(name, object);
    m_options.set(m_map.keySet().toArray(new String[0]));
  }

  /**
   * Adds the given object to the list of options and marks it as the default.
   *
   * @param name the name of the option
   * @param object the option
   */
  public void addDefault(String name, V object) {
    add(name, object);
    setDefault(name);
  }

  /**
   * Marks the given option as the default.
   *
   * @param name the name of the option
   */
  public void setDefault(String name) {
    requireNonNullParam(name, "name", "setDefault");
    m_defaultChoice.set(name);
  }

  /**
   * Clears the list of options and resets the default. Does not change the selected option;
   * getSelected() will return null after this is called until a new matching option is added.
   */
  public void clear() {
    m_map.clear();
    m_options.set(new String[0]);
    m_defaultChoice.set("");
  }

  /**
   * Returns the selected option. If there is none selected, it will return the default. If there is
   * none selected and no default, then it will return {@code null}.
   *
   * @return the option selected
   */
  public V getSelected() {
    String selected = m_selected.get();
    if (selected != null && !selected.isEmpty()) {
      return m_map.get(selected);
    } else {
      return m_map.get(m_defaultChoice.get());
    }
  }

  /**
   * Bind a listener that's called when the selected value changes. Only one listener can be bound.
   * Calling this function will replace the previous listener.
   *
   * @param listener The function to call that accepts the new value
   */
  public void onChange(Consumer<V> listener) {
    requireNonNullParam(listener, "listener", "onChange");
    m_listener = listener;
  }

  @Override
  public void publishTunable(TunableTable table) {
    table.publish(DEFAULT, m_defaultChoice);
    table.publish(OPTIONS, m_options);
    table.publish(SELECTED, m_selected);
  }

  @Override
  public String getTunableType() {
    return "Selectable";
  }
}
