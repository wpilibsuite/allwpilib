// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import java.util.Set;

record PriorityCommand(int priority, Mechanism... subsystems) implements Command {
  @Override
  public void run(Coroutine coroutine) {
    coroutine.park();
  }

  @Override
  public Set<Mechanism> requirements() {
    return Set.of(subsystems);
  }

  @Override
  public String name() {
    return toString();
  }

  @Override
  public String toString() {
    return "PriorityCommand[priority=" + priority + ", subsystems=" + Set.of(subsystems) + "]";
  }
}
