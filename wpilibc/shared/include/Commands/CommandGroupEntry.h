/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef __COMMAND_GROUP_ENTRY_H__
#define __COMMAND_GROUP_ENTRY_H__

class Command;

class CommandGroupEntry {
 public:
  typedef enum {
    kSequence_InSequence,
    kSequence_BranchPeer,
    kSequence_BranchChild
  } Sequence;

  CommandGroupEntry() = default;
  CommandGroupEntry(Command *command, Sequence state, double timeout = -1.0);
  bool IsTimedOut() const;

  double m_timeout = -1.0;
  Command *m_command = nullptr;
  Sequence m_state = kSequence_InSequence;
};

#endif
