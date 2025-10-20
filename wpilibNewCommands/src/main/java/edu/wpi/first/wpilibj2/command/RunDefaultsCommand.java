// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;

class RunDefaultsCommand extends Command {
  private final Set<Subsystem> subsystems;
  private final List<Command> defaultCmds = new ArrayList<>();

  public RunDefaultsCommand(Subsystem... subsystems) {
    this.subsystems = Set.of(subsystems);
  }

  public RunDefaultsCommand(Set<Subsystem> subsystems) {
    this.subsystems = subsystems;
  }

  @Override
  public void initialize() {
    for (var s : subsystems) {
      var defaultCmd = s.getDefaultCommand();
      if (defaultCmd != null) {
        defaultCmds.add(defaultCmd);
        defaultCmd.initialize();
      }
    }
  }

  @Override
  public void execute() {
    for (var cmd : defaultCmds) {
      cmd.execute();
    }
  }
}
