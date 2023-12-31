package edu.wpi.first.wpilibj2.command.async;

import java.util.ArrayList;
import java.util.List;

public record Group<T>(List<Stage<T>> stages) {
  public Group(List<Stage<T>> stages) {
    this.stages = List.copyOf(stages);
  }

  public Group<T> andThen(Group<T> other) {
    var allStages = new ArrayList<>(this.stages);
    allStages.addAll(other.stages);
    return new Group<>(allStages);
  }

  public Group<T> andThen(Stage<T> next) {
    var allStages = new ArrayList<>(this.stages);
    allStages.add(next);
    return new Group<>(allStages);
  }

  public Group<T> andThen(T next) {
    var allStages = new ArrayList<>(this.stages);
    allStages.add(Stage.all(next));
    return new Group<>(allStages);
  }
}
