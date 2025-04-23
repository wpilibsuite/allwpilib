// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.commandsv3;

import java.util.Set;

class NullCommand implements Command {
  @Override
  public void run(Coroutine coroutine) {
    coroutine.park();
  }

  @Override
  public String name() {
    return "Null Command";
  }

  @Override
  public Set<RequireableResource> requirements() {
    return Set.of();
  }
}
