/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.experimental.controller;

import java.util.concurrent.locks.ReentrantLock;

import edu.wpi.first.wpilibj.Notifier;

public class ControllerRunner {
  private final Notifier m_notifier = new Notifier(this::run);
  private final Controller m_controller;
  private final ControllerOutput m_controllerOutput;
  private boolean m_enabled;

  private final ReentrantLock m_thisMutex = new ReentrantLock();

  // Ensures when disable() is called, m_controllerOutput() won't run if
  // Controller.update() is already running at that time.
  private final ReentrantLock m_outputMutex = new ReentrantLock();

  /**
   * Allocate a ControllerRunner.
   *
   * @param controller       The controller on which to call update().
   * @param controllerOutput The object which updates the plant using the
   *                         controller output passed as the argument.
   */
  public ControllerRunner(Controller controller, ControllerOutput controllerOutput) {
    m_controller = controller;
    m_controllerOutput = controllerOutput;
    m_notifier.startPeriodic(m_controller.getPeriod());
  }

  /**
   * Begin running the controller.
   */
  public void enable() {
    m_thisMutex.lock();
    try {
      m_enabled = true;
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Stop running the controller.
   *
   * <p>This sets the output to zero before stopping.
   */
  public void disable() {
    // Ensures m_enabled modification and m_controllerOutput() call occur
    // atomically
    m_outputMutex.lock();
    try {
      m_thisMutex.lock();
      try {
        m_enabled = false;
      } finally {
        m_thisMutex.unlock();
      }

      m_controllerOutput.setOutput(0.0);
    } finally {
      m_outputMutex.unlock();
    }
  }

  /**
   * Returns whether controller is running.
   */
  public boolean isEnabled() {
    m_thisMutex.lock();
    try {
      return m_enabled;
    } finally {
      m_thisMutex.unlock();
    }
  }


  private void run() {
    // Ensures m_enabled check and m_controllerOutput() call occur atomically
    m_outputMutex.lock();
    try {
      m_thisMutex.lock();
      try {
        if (m_enabled) {
          // Don't block other ControllerRunner operations on output
          m_thisMutex.unlock();

          m_controllerOutput.setOutput(m_controller.update());
        }
      } finally {
        if (m_thisMutex.isHeldByCurrentThread()) {
          m_thisMutex.unlock();
        }
      }
    } finally {
      m_outputMutex.unlock();
    }
  }
}
