// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <units/time.h>

namespace frc {

class Command;

class CommandGroupEntry {
 public:
  enum Sequence {
    kSequence_InSequence,
    kSequence_BranchPeer,
    kSequence_BranchChild
  };

  CommandGroupEntry() = default;
  CommandGroupEntry(Command* command, Sequence state,
                    units::second_t timeout = -1_s);

  CommandGroupEntry(CommandGroupEntry&&) = default;
  CommandGroupEntry& operator=(CommandGroupEntry&&) = default;

  bool IsTimedOut() const;

  units::second_t m_timeout = -1_s;
  Command* m_command = nullptr;
  Sequence m_state = kSequence_InSequence;
};

}  // namespace frc
