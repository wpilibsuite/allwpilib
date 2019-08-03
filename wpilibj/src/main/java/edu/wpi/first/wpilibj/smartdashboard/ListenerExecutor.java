/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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

  /**
   * Runs all posted tasks.  Called periodically from main thread.
   */
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
