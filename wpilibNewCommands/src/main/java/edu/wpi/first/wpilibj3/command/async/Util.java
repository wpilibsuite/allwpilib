package edu.wpi.first.wpilibj3.command.async;

import java.util.concurrent.atomic.AtomicReference;
import java.util.concurrent.locks.ReadWriteLock;
import java.util.function.Supplier;

/**
 * Miscellaneous utility class for the async command API.
 */
public final class Util {
  public static final AtomicReference<Thread> currentLockOwner = new AtomicReference<>(null);

  private Util() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Performs a read operation using a {@code ReadWriteLock}, automatically locking and unlocking.
   * The result of the read operation is returned.
   *
   * @param lock the lock to read with
   * @param task the read task to perform
   * @param <T>  the type of data that may be returned by the task
   * @return the result of the read task
   */
  public static <T> T reading(ReadWriteLock lock, Supplier<? extends T> task) {
    Logger.log("LOCK", "Attempting to acquire read lock... (current owner: " + currentLockOwner.get() + ")");
    lock.readLock().lock();
    Logger.log("LOCK", "Read lock acquired by " + Thread.currentThread().getName());
    currentLockOwner.compareAndSet(null, Thread.currentThread());
    try {
      return task.get();
    } finally {
      if (currentLockOwner.compareAndSet(Thread.currentThread(), null)) {
        Logger.log("LOCK", "Read lock released by " + Thread.currentThread().getName());
      }
      lock.readLock().unlock();
    }
  }

  /**
   * Performs a read operation using a {@code ReadWriteLock}, automatically locking and unlocking.
   *
   * @param lock the lock to read with
   * @param task the read task to perform
   */
  public static void reading(ReadWriteLock lock, Runnable task) {
    reading(
        lock,
        () -> {
          task.run();
          return null;
        });
  }

  /**
   * Performs a write operation using a {@code ReadWriteLock}, automatically locking and unlocking.
   * The result of the write operation is returned.
   *
   * @param lock the lock to write with
   * @param task the write task to perform
   * @param <T>  the type of data that may be returned by the task
   * @return the result of the write task
   */
  public static <T> T writing(ReadWriteLock lock, Supplier<? extends T> task) {
    Logger.log("LOCK", "Attempting to acquire write lock... (current owner: " + currentLockOwner.get() + ")");
    lock.writeLock().lock();
    Logger.log("LOCK", "Write lock acquired by " + Thread.currentThread().getName());
    currentLockOwner.compareAndSet(null, Thread.currentThread());
    try {
      return task.get();
    } finally {
      if (currentLockOwner.compareAndSet(Thread.currentThread(), null)) {
        Logger.log("LOCK", "Write lock released by " + Thread.currentThread().getName());
      }
      lock.writeLock().unlock();
    }
  }

  /**
   * Performs a write operation using a {@code ReadWriteLock}, automatically locking and unlocking.
   *
   * @param lock the lock to write with
   * @param task the write task to perform
   */
  public static void writing(ReadWriteLock lock, Runnable task) {
    writing(
        lock,
        () -> {
          task.run();
          return null;
        });
  }
}
