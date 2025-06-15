// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import static edu.wpi.first.units.Units.Seconds;
import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.units.measure.Time;
import edu.wpi.first.wpilibj.Timer;
import java.util.Set;

/** A command with no requirements that merely waits for a specified duration of time to elapse. */
public class WaitCommand implements Command {
  private final Time m_duration;

  public WaitCommand(Time duration) {
    m_duration = requireNonNullParam(duration, "duration", "WaitCommand");
  }

  @Override
  public void run(Coroutine coroutine) {
    var timer = new Timer();
    timer.start();
    while (!timer.hasElapsed(m_duration.in(Seconds))) {
      coroutine.yield();
    }
  }

  @Override
  public String name() {
    return "Wait " + m_duration.in(Seconds) + " Seconds";
  }

  @Override
  public Set<RequireableResource> requirements() {
    return Set.of();
  }
}
