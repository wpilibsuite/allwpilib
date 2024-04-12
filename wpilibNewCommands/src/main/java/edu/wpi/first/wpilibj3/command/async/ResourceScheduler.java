package edu.wpi.first.wpilibj3.command.async;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Optional;
import java.util.Set;
import java.util.concurrent.Callable;
import java.util.concurrent.CancellationException;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class ResourceScheduler {
  public interface Res {}

  private record ExecutionData<V>(Res resource, Lock lock, Callable<V> task, Future<V> future) {}

  private final Set<Res> resources = new HashSet<>();

  private final Map<Res, ExecutionData<?>> executionData = new HashMap<>();

  private final ExecutorService executor = Executors.newVirtualThreadPerTaskExecutor();

  public void addResource(Res resource) {
    this.resources.add(resource);
    executionData.put(resource, new ExecutionData<>(resource, new ReentrantLock(), null, null));
  }

  public <T> Optional<Future<T>> schedule(Res res, Callable<T> task) {
    var data = executionData.get(res);

    data.lock.lock();

    try {
      if (data.future != null) {
        data.future.cancel(true);
        try {
          data.future.get(10, TimeUnit.MILLISECONDS);
        } catch (InterruptedException
            | ExecutionException
            | CancellationException
            | TimeoutException e) {
          // Couldn't schedule
          return Optional.empty();
        }
      }

      var future =
          executor.submit(
              () -> {
                data.lock.lockInterruptibly();
                try {
                  return task.call();
                } finally {
                  data.lock.unlock();
                }
              });

      executionData.put(res, new ExecutionData<T>(res, data.lock, task, future));
      return Optional.of(future);
    } finally {
      data.lock.unlock();
    }
  }
}
