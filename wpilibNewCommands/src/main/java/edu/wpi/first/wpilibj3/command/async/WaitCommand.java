package edu.wpi.first.wpilibj3.command.async;

import static edu.wpi.first.units.Units.Seconds;

import edu.wpi.first.units.Measure;
import edu.wpi.first.units.Time;
import edu.wpi.first.wpilibj.Timer;
import java.util.Set;

/** A command with no requirements that merely waits for a specified duration of time to elapse. */
public class WaitCommand implements AsyncCommand {
  private final Measure<Time> duration;

  public WaitCommand(Measure<Time> duration) {
    this.duration = duration;
  }

  @Override
  public void run(Coroutine coroutine) {
    var timer = new Timer();
    timer.start();
    while (!timer.hasElapsed(duration.in(Seconds))) {
      coroutine.yield();
    }
  }

  @Override
  public String name() {
    return "Wait " + duration.in(Seconds) + " Seconds";
  }

  @Override
  public Set<HardwareResource> requirements() {
    return Set.of();
  }
}
