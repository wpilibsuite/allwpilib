package edu.wpi.first.wpilibj3.command.async;

import static edu.wpi.first.units.Units.Seconds;

import edu.wpi.first.units.Measure;
import edu.wpi.first.units.MutableMeasure;
import edu.wpi.first.units.Time;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.Objects;
import java.util.Set;

public final class Stage {
  private final Collection<AsyncCommand> commands;
  private Set<AsyncCommand> requiredCommands;
  // Default timeout is 10e+308 seconds (essentially, unbounded)
  private final MutableMeasure<Time> timeout = MutableMeasure.ofBaseUnits(Double.MAX_VALUE, Seconds);

  public Stage(Collection<AsyncCommand> commands, Set<AsyncCommand> requiredCommands) {
    this.commands = Set.copyOf(commands);
    this.requiredCommands = Set.copyOf(requiredCommands);
  }

  public static  Stage all(AsyncCommand... commands) {
    var c = Set.of(commands);
    return new Stage(c, c);
  }

  public static Stage race(AsyncCommand... commands) {
    var c = Set.of(commands);
    return new Stage(c, Set.of());
  }

  public Stage merge(Stage other) {
    var allCommands = new ArrayList<AsyncCommand>(commands.size() + other.commands.size());
    allCommands.addAll(commands);
    allCommands.addAll(other.commands);
    return new Stage(allCommands, Util.combineSets(requiredCommands, other.requiredCommands));
  }

  public Stage untilAllFinish() {
    requiredCommands = Set.copyOf(commands);
    return this;
  }

  public Stage untilAnyFinish() {
    requiredCommands = Set.of();
    return this;
  }

  public Stage untilDeadlineFinishes(AsyncCommand deadline) {
    requiredCommands = Set.of(deadline);
    return this;
  }

  public Collection<AsyncCommand> commands() {
    return commands;
  }

  public Set<AsyncCommand> getRequiredCommands() {
    return requiredCommands;
  }

  public Measure<Time> getTimeout() {
    return timeout;
  }

  public Stage withTimeout(Measure<Time> timeout) {
    this.timeout.mut_replace(timeout);
    return this;
  }

  public Group andThen(Stage other) {
    return new Group(List.of(this, other));
  }

  public Group andThen(AsyncCommand other) {
    return andThen(Stage.all(other));
  }

  @Override
  public String toString() {
    return "Stage{" +
        "commands=" + commands +
        ", requiredCommands=" + requiredCommands +
        ", timeout=" + timeout +
        '}';
  }

  @Override
  public boolean equals(Object o) {
    if (this == o) return true;
    if (o == null || getClass() != o.getClass()) return false;
    Stage stage = (Stage) o;
    return Objects.equals(commands, stage.commands)
        && Objects.equals(requiredCommands, stage.requiredCommands)
        && Objects.equals(timeout, stage.timeout);
  }

  @Override
  public int hashCode() {
    return Objects.hash(commands, requiredCommands, timeout);
  }
}
