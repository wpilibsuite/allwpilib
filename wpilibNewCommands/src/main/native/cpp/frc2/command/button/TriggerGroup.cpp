// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/button/TriggerGroup.h"

#include <stdexcept>
#include <set>
#include <utility>

using namespace frc2;

TriggerGroup TriggerGroup::Add(const std::string& key, Trigger condition) const {
  if (key.empty()) {
    throw std::invalid_argument("Key must not be empty in Add()");
  }
  if (m_conditions.find(key) != m_conditions.end()) {
    throw std::invalid_argument("Key '" + key + "' already exists in TriggerGroup");
  }

  auto newConditions = m_conditions;
  //newConditions[key] = std::move(condition);
  newConditions.emplace(key, condition);
  return TriggerGroup(std::move(newConditions));
}

Trigger TriggerGroup::Only(const std::vector<std::string>& names) const {
  CheckKeysExist(names);
  std::set<std::string> expected(names.begin(), names.end());

  return Trigger([expected, conditions = m_conditions]() {
    for (const auto& [key, trigger] : conditions) {
      bool shouldBeTrue = expected.find(key) != expected.end();
      if (trigger.Get() != shouldBeTrue) {
        return false;
      }
    }
    return true;
  });
}

Trigger TriggerGroup::Any(const std::vector<std::string>& names) const {
  CheckKeysExist(names);

  return Trigger([names, conditions = m_conditions]() {
    for (const auto& name : names) {
      if (conditions.at(name).Get()) {
        return true;
      }
    }
    return false;
  });
}

Trigger TriggerGroup::AllOf(const std::vector<std::string>& names) const {
  CheckKeysExist(names);

  return Trigger([names, conditions = m_conditions]() {
    for (const auto& name : names) {
      if (!conditions.at(name).Get()) {
        return false;
      }
    }
    return true;
  });
}

Trigger TriggerGroup::All() const {
  std::vector<std::string> keys;
  keys.reserve(m_conditions.size());
  for (const auto& [key, _] : m_conditions) {
    keys.push_back(key);
  }
  return AllOf(keys);
}

Trigger TriggerGroup::None() const {
  return Trigger([conditions = m_conditions]() {
    for (const auto& [_, trigger] : conditions) {
      if (trigger.Get()) {
        return false;
      }
    }
    return true;
  });
}

void TriggerGroup::CheckKeysExist(const std::vector<std::string>& keys) const {
  for (const auto& key : keys) {
    if (m_conditions.find(key) == m_conditions.end()) {
      throw std::invalid_argument("Key '" + key + "' does not exist in TriggerGroup");
    }
  }
}