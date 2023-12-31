package edu.wpi.first.wpilibj2.command.async;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.concurrent.Callable;
import java.util.concurrent.locks.ReadWriteLock;
import java.util.function.Supplier;

public class Util {
  @SafeVarargs
  public static <T> Set<T> combineSets(Set<? extends T>... sets) {
    var result = new HashSet<T>();
    for (Set<? extends T> set : sets) {
      result.addAll(set);
    }
    return result;
  }

  @SafeVarargs
  public static <T> List<T> concat(List<? extends T>... lists) {
    var result = new ArrayList<T>();
    for (List<? extends T> list : lists) {
      result.addAll(list);
    }
    return result;
  }

  public static <T> T reading(ReadWriteLock lock, Supplier<? extends T> task) {
    lock.readLock().lock();
    try {
      return task.get();
    } finally {
      lock.readLock().unlock();
    }
  }

  public static void reading(ReadWriteLock lock, Runnable task) {
    reading(lock, () -> {
      task.run();
      return null;
    });
  }

  public static <T> T writing(ReadWriteLock lock, Supplier<? extends T> task) {
    lock.writeLock().lock();
    try {
      return task.get();
    } finally {
      lock.writeLock().unlock();
    }
  }

  public static void writing(ReadWriteLock lock, Runnable task) {
    writing(lock, () -> {
      task.run();
      return null;
    });
  }
}
