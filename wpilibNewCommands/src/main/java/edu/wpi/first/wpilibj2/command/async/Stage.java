package edu.wpi.first.wpilibj2.command.async;

import edu.wpi.first.units.Measure;
import edu.wpi.first.units.Time;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.Set;

public final class Stage<T> {
  private final Collection<T> commands;
  private Set<T> requiredCommands;
  private Measure<Time> timeout = null;

  public Stage(Collection<T> commands, Set<T> requiredCommands) {
    this.commands = Set.copyOf(commands);
    this.requiredCommands = Set.copyOf(requiredCommands);
  }

  @SafeVarargs
  public static <T> Stage<T> all(T... commands) {
    Set<T> c = Set.of(commands);
    return new Stage<>(c, c);
  }

  @SafeVarargs
  public static <T> Stage<T> race(T... commands) {
    Set<T> c = Set.of(commands);
    return new Stage<>(c, Set.of());
  }

  public Measure<Time> getTimeout() {
    return timeout;
  }

  public Stage<T> merge(Stage<T> other) {
    var allCommands = new ArrayList<T>(commands.size() + other.commands.size());
    allCommands.addAll(commands);
    allCommands.addAll(other.commands);
    return new Stage<>(allCommands, Util.combineSets(requiredCommands, other.requiredCommands));
  }

  public Stage<T> untilAllFinish() {
    requiredCommands = Set.copyOf(commands);
    return this;
  }

  public Stage<T> untilAnyFinish() {
    requiredCommands = Set.of();
    return this;
  }

  public Stage<T> untilDeadlineFinishes(T deadline) {
    requiredCommands = Set.of(deadline);
    return this;
  }

  public Collection<T> commands() {
    return commands;
  }

  public Set<T> getRequiredCommands() {
    return requiredCommands;
  }

  public Stage<T> withTimeout(Measure<Time> timeout) {
    this.timeout = timeout;
    return this;
  }

  public Group<T> andThen(Stage<T> other) {
    return new Group<>(List.of(this, other));
  }

  public Group<T> andThen(T other) {
    return andThen(Stage.all(other));
  }
}
