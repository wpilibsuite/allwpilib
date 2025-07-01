// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.autodiff;

import java.util.ArrayDeque;
import java.util.Deque;
import org.wpilib.util.ErrorMessages;
import org.wpilib.util.cleanup.CleanupPool;

/**
 * Cleans up implicitly allocated Variables via try-with-resources.
 *
 * <p>This implements a stack of Variable pools containing a default global pool. The user can
 * create additional pools via try-with-resources. Variable and VariableMatrix instances will
 * register themselves with the latest pool.
 *
 * <p>It's strongly recommended to only instantiate this class via try-with-resources so the close()
 * methods are always called in the correct order (i.e., nested scopes).
 */
public class VariablePool implements AutoCloseable {
  private static Deque<VariablePool> s_variablePoolStack = new ArrayDeque<VariablePool>();

  // Default global pool
  @SuppressWarnings("PMD.UnusedPrivateField")
  private static VariablePool s_globalPool = new VariablePool();

  // Cleans up Variables in the scope of this VariablePool
  private final CleanupPool m_cleanupPool = new CleanupPool();

  /** Default constructor. */
  @SuppressWarnings("this-escape")
  public VariablePool() {
    s_variablePoolStack.addFirst(this);
  }

  @Override
  public void close() {
    m_cleanupPool.close();
    s_variablePoolStack.removeFirst();
  }

  /**
   * Registers a Variable in the Variable stack for cleanup.
   *
   * @param variable The Variable to register
   * @return The registered Variable
   */
  public static Variable register(Variable variable) {
    ErrorMessages.requireNonNullParam(variable, "variable", "register");
    s_variablePoolStack.getFirst().m_cleanupPool.register(variable);
    return variable;
  }
}
