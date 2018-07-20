/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

namespace frc {

class Command;

class CommandGroupEntry {
 public:
  typedef enum {
    kSequence_InSequence,
    kSequence_BranchPeer,
    kSequence_BranchChild
  } Sequence;

  CommandGroupEntry() = default;
  CommandGroupEntry(Command* command, Sequence state, double timeout = -1.0);
  bool IsTimedOut() const;

  double m_timeout = -1.0;
  Command* m_command = nullptr;
  Sequence m_state = kSequence_InSequence;
};

}  // namespace frc
