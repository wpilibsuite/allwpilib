// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command.button;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import java.util.HashMap;
import java.util.Map;
import java.util.Set;
import java.util.function.BooleanSupplier;

/**
 * This class provides an easy way to combine multiple Triggers together in a single group. Each
 * Trigger in a group is associated with a unique String identifier.
 *
 * <p>A common pattern is to combine multiple joystick buttons to perform certain actions, while the
 * individual buttons perform individual actions. For example, the 'a' and 'b' buttons could be used
 * to perform 3 actions: one when just a is pressed, one when just b is pressed, and one when both
 * are pressed.
 *
 * <p>This class is provided by the NewCommands VendorDep
 */
public class TriggerGroup {
  private final Map<String, BooleanSupplier> m_conditions;

  /**
   * Creates an empty TriggerGroup. This can be used as a starting point for creating TriggerGroups
   * by chaining the {@link #add(String, BooleanSupplier)} method.
   */
  public TriggerGroup() {
    this(Map.of());
  }

  /**
   * Creates a TriggerGroup from the given map of named conditions. The map is copied to ensure
   * immutability.
   *
   * @param conditions A map of named conditions (Triggers).
   */
  public TriggerGroup(Map<String, BooleanSupplier> conditions) {
    m_conditions = Map.copyOf(conditions);
  }

  /**
   * Creates a new TriggerGroup with the same conditions as this one, plus an additional named
   * condition. This method does not modify the original TriggerGroup and can be chained.
   *
   * @param key The String identifier associated with the condition.
   * @param condition The condition to add.
   * @return A new TriggerGroup.
   */
  public TriggerGroup add(String key, BooleanSupplier condition) {
    requireNonNullParam(key, "key", "add");
    requireNonNullParam(condition, "condition", "add");

    if (m_conditions.containsKey(key)) {
      throw new IllegalArgumentException("Key " + key + " already exists in TriggerGroup");
    }

    HashMap<String, BooleanSupplier> newConditions = new HashMap<>(m_conditions);
    newConditions.put(key, condition);
    return new TriggerGroup(newConditions);
  }

  /**
   * Creates a Trigger that becomes true only when the specified named conditions are true and all
   * other conditions in the group are false.
   *
   * @param names The names of the conditions that must be true. All other conditions in the group
   *     must be false.
   * @return A Trigger that is true if and only if exactly the specified conditions are true.
   */
  public Trigger only(String... names) {
    checkKeysExist(names);
    Set<String> expected = Set.of(names);

    return new Trigger(
        () -> {
          for (var condition : m_conditions.entrySet()) {
            boolean expectedValue = expected.contains(condition.getKey());
            boolean value = condition.getValue().getAsBoolean();

            if (expectedValue != value) {
              return false;
            }
          }

          return true;
        });
  }

  /**
   * Creates a Trigger that becomes true when any of the specified named conditions are true.
   *
   * @param names The names of the conditions to check.
   * @return A Trigger that is true only if at least one of the specified conditions are true.
   */
  public Trigger any(String... names) {
    checkKeysExist(names);

    return new Trigger(
        () -> {
          for (var name : names) {
            if (m_conditions.get(name).getAsBoolean()) {
              return true;
            }
          }

          return false;
        });
  }

  /**
   * Creates a Trigger that becomes true when all of the specified named conditions are true,
   * regardless of the others in this group.
   *
   * @param names The names of the conditions that must be true.
   * @return A Trigger that is true only if all of the specified conditions are true, regardless of
   *     the others.
   */
  public Trigger allOf(String... names) {
    checkKeysExist(names);

    return new Trigger(
        () -> {
          for (var name : names) {
            if (!m_conditions.get(name).getAsBoolean()) {
              return false;
            }
          }

          return true;
        });
  }

  /**
   * Creates a Trigger that becomes true only when every single condition in this group is true.
   *
   * @return A Trigger that is true if every single condition in this group is true.
   */
  public Trigger all() {
    return allOf(m_conditions.keySet().toArray(new String[0]));
  }

  /**
   * Creates a Trigger that becomes true only when every single condition in this group is false.
   *
   * @return A Trigger that is true if every single condition in this group is false.
   */
  public Trigger none() {
    return new Trigger(
        () -> {
          for (var condition : m_conditions.values()) {
            if (condition.getAsBoolean()) {
              return false;
            }
          }

          return true;
        });
  }

  /**
   * Helper method that checks that all the provided keys do exist in this TriggerGroup, throwing if
   * one does not.
   *
   * @param keys The list of keys to check.
   */
  private void checkKeysExist(String... keys) {
    for (String k : keys) {
      if (!m_conditions.containsKey(k)) {
        throw new IllegalArgumentException("Key " + k + " does not exist in TriggerGroup");
      }
    }
  }
}
