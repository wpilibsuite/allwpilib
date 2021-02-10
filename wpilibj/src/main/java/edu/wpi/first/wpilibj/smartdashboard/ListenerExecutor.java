// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.smartdashboard;

import java.util.ArrayList;
import java.util.Collection;
import java.util.concurrent.Executor;

/**
 * An executor for running listener tasks posted by {@link edu.wpi.first.wpilibj.Sendable} listeners
 * synchronously from the main application thread.
 */
class ListenerExecutor implements Executor {
  private final Collection<Runnable> m_tasks = new ArrayList<>();
  private final Object m_lock = new Object();

  /**
   * Posts a task to the executor to be run synchronously from the main thread.
   *
   * @param task The task to run synchronously from the main thread.
   */
  @Override
  public void execute(Runnable task) {
    synchronized (m_lock) {
      m_tasks.add(task);
    }
  }

  /** Runs all posted tasks. Called periodically from main thread. */
  public void runListenerTasks() {
    // Locally copy tasks from internal list; minimizes blocking time
    Collection<Runnable> tasks = new ArrayList<>();
    synchronized (m_lock) {
      tasks.addAll(m_tasks);
      m_tasks.clear();
    }

    // Run all tasks
    for (Runnable task : tasks) {
      task.run();
    }
  }
}
