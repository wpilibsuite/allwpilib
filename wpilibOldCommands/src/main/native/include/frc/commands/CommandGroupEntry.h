// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

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
  CommandGroupEntry(Command* command, Sequence state, double timeout = -1.0);

  CommandGroupEntry(CommandGroupEntry&&) = default;
  CommandGroupEntry& operator=(CommandGroupEntry&&) = default;

  bool IsTimedOut() const;

  double m_timeout = -1.0;
  Command* m_command = nullptr;
  Sequence m_state = kSequence_InSequence;
};

}  // namespace frc
