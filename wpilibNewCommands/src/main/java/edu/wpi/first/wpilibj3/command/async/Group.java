package edu.wpi.first.wpilibj3.command.async;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;
import java.util.concurrent.TimeoutException;
import java.util.stream.Collectors;

public record Group(List<Stage> stages) {
  public Group(List<Stage> stages) {
    this.stages = List.copyOf(stages);
  }

  public Group andThen(Group other) {
    var allStages = new ArrayList<>(this.stages);
    allStages.addAll(other.stages);
    return new Group(allStages);
  }

  public Group andThen(Stage next) {
    var allStages = new ArrayList<>(this.stages);
    allStages.add(next);
    return new Group(allStages);
  }

  public Group andThen(AsyncCommand next) {
    var allStages = new ArrayList<>(this.stages);
    allStages.add(Stage.all(next));
    return new Group(allStages);
  }
}
