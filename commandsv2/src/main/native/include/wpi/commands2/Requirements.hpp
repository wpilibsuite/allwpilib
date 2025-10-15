// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <initializer_list>
#include <span>
#include <vector>

#include "frc2/command/Subsystem.h"

namespace frc2 {

/**
 * Represents requirements for a command, which is a set of (pointers to)
 * subsystems. This class is implicitly convertible from std::initializer_list
 * and std::span.
 */
class Requirements {
 public:
  // NOLINTNEXTLINE
  /*implicit*/ Requirements(std::initializer_list<Subsystem*> requirements)
      : m_subsystems{requirements.begin(), requirements.end()} {}

  // NOLINTNEXTLINE
  /*implicit*/ Requirements(std::span<Subsystem* const> requirements)
      : m_subsystems{requirements.begin(), requirements.end()} {}

  Requirements() = default;

  Requirements(const Requirements&) = default;

  std::vector<Subsystem*>::const_iterator begin() const {
    return m_subsystems.begin();
  }

  std::vector<Subsystem*>::const_iterator end() const {
    return m_subsystems.end();
  }

 private:
  std::vector<Subsystem*> m_subsystems;
};

}  // namespace frc2
