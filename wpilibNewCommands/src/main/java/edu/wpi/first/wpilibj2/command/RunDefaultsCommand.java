// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;

class RunDefaultsCommand extends Command {
  private final Set<Subsystem> m_subsystems;
  private final List<Command> m_defaultCmds = new ArrayList<>();

  RunDefaultsCommand(Subsystem... subsystems) {
    m_subsystems = Set.of(subsystems);
  }

  RunDefaultsCommand(Set<Subsystem> subsystems) {
    m_subsystems = subsystems;
  }

  @Override
  public void initialize() {
    for (var s : m_subsystems) {
      var defaultCmd = s.getDefaultCommand();
      if (defaultCmd != null) {
        m_defaultCmds.add(defaultCmd);
        defaultCmd.initialize();
      }
    }
  }

  @Override
  public void execute() {
    for (var cmd : m_defaultCmds) {
      cmd.execute();
    }
  }
}
