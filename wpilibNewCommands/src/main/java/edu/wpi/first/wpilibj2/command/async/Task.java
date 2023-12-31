package edu.wpi.first.wpilibj2.command.async;

import edu.wpi.first.util.ErrorMessages;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.concurrent.Callable;

/**
 * @param name     the name of the task
 * @param resource the resource required by the task
 * @param priority the priority of the task. Larger numbers correspond to higher priority; a task
 *                 will not be able to cancel another task with a larger priority value.
 * @param impl     the implementation of the task
 * @param <R>      the type of the resource required by the task
 * @param <T>      the type of values returned by the task upon completion
 */
public record Task<R extends Resource<R>, T>(String name,
                                             R resource,
                                             int priority,
                                             Callable<T> impl) implements Callable<T> {
  public static final int DEFAULT_PRIORITY = 0;
  public static final int LOWEST_PRIORITY = Integer.MIN_VALUE;
  public static final int HIGHEST_PRIORITY = Integer.MAX_VALUE;

  public Task {
    ErrorMessages.requireNonNullParam(name, "name", "Task");
    ErrorMessages.requireNonNullParam(resource, "resource", "Task");
    ErrorMessages.requireNonNullParam(impl, "impl", "Task");
  }

  public Task(String name, R resource, Callable<T> impl) {
    this(name, resource, DEFAULT_PRIORITY, impl);
  }

  public static <R extends Resource<R>, T> Task<R, List<T>> seq(String name, Group<Task<R, T>> group) {
    var resources = new HashSet<R>();
    int priority = LOWEST_PRIORITY;

    for (var stage : group.stages()) {
      for (var task : stage.commands()) {
        resources.add(task.resource);
        priority = Math.max(priority, task.priority);
      }
    }

    if (resources.size() != 1) {
      throw new IllegalArgumentException();
    }

    var resource = resources.iterator().next();

    return new Task<>(
        name,
        resource,
        priority,
        () -> {
          List<T> results = new ArrayList<>(group.stages().size());

          for (var stage : group.stages()) {
            // known to have a single task
            var task = stage.commands().iterator().next();
            resource.runTask(task);

            @SuppressWarnings("unchecked")
            T result = (T) resource.await(); // safe cast - the task is known to return T

            results.add(result);
          }

          return results;
        }
    );
  }

  @Override
  public T call() throws Exception {
    return impl.call();
  }

  @Override
  public String toString() {
    return "Task[" +
        "name='" + name + '\'' +
        ", resource=" + resource.getName() +
        ']';
  }

  public static class Builder<R extends Resource<R>, T> {
    private final R resource;
    private String name;

    public Builder(R resource) {
      this.resource = resource;
    }

    public Builder<R, T> named(String name) {
      this.name = name;
      return this;
    }

    public Task<R, T> running(Callable<T> impl) {
      var name = this.name;

      return new Task<>(name, resource, impl);
    }
  }
}
