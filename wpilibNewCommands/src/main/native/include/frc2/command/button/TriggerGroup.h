// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc2/command/button/Trigger.h>

#include <functional>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace frc2 {

/**
 * This class provides an easy way to combine multiple Triggers together in a
 * single group. Each Trigger in a group is associated with a unique String
 * identifier.
 *
 * <p>A common pattern is to combine multiple joystick buttons to perform
 * certain actions, while the individual buttons perform individual actions. For
 * example, the 'a' and 'b' buttons could be used to perform 3 actions: one when
 * just a is pressed, one when just b is pressed, and one when both are pressed.
 *
 * <p>This class is provided by the NewCommands VendorDep
 */
class TriggerGroup {
 public:
  /**
   * Creates an empty TriggerGroup. This can be used as a starting point for
   * creating TriggerGroups by chaining the add(String, Trigger) method.
   */
  TriggerGroup() = default;

  /**
   * Creates a TriggerGroup from the given map of named conditions. The map is
   * copied to ensure immutability.
   *
   * @param conditions A map of named conditions (Triggers).
   */
  explicit TriggerGroup(std::map<std::string, Trigger> conditions)
      : m_conditions(std::move(conditions)) {}

  /**
   * Creates a new TriggerGroup with the same conditions as this one, plus an
   * additional named condition. This method does not modify the original
   * TriggerGroup and can be chained.
   *
   * @param key The String identifier associated with the condition.
   * @param condition The condition to add.
   * @return A new TriggerGroup.
   */
  TriggerGroup Add(const std::string& key, Trigger condition) const;

  /**
   * Creates a Trigger that becomes true only when the specified named
   * conditions are true and all other conditions in the group are false.
   *
   * @param names The names of the conditions that must be true. All other
   * conditions in the group must be false.
   * @return A Trigger that is true if and only if exactly the specified
   * conditions are true.
   */
  Trigger Only(const std::vector<std::string>& names) const;

  /**
   * Creates a Trigger that becomes true when any of the specified named
   * conditions are true.
   *
   * @param names The names of the conditions to check.
   * @return A Trigger that is true only if at least one of the specified
   * conditions are true.
   */
  Trigger Any(const std::vector<std::string>& names) const;

  /**
   * Creates a Trigger that becomes true when all of the specified named
   * conditions are true, regardless of the others in this group.
   *
   * @param names The names of the conditions that must be true.
   * @return A Trigger that is true only if all of the specified conditions are
   * true, regardless of the others.
   */
  Trigger AllOf(const std::vector<std::string>& names) const;

  /**
   * Creates a Trigger that becomes true only when every single condition in
   * this group is true.
   *
   * @return A Trigger that is true if every single condition in this group is
   * true.
   */
  Trigger All() const;

  /**
   * Creates a Trigger that becomes true only when every single condition in
   * this group is false.
   *
   * @return A Trigger that is true if every single condition in this group is
   * false.
   */
  Trigger None() const;

 private:
  std::map<std::string, Trigger> m_conditions;

  /**
   * Helper method that checks that all the provided keys do exist in this
   * TriggerGroup, throwing if one does not.
   *
   * @param keys The list of keys to check.
   */
  void CheckKeysExist(const std::vector<std::string>& keys) const;
};

}  // namespace frc2
