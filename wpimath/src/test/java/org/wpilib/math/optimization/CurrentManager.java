// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.optimization;

import static org.wpilib.math.optimization.Constraints.ge;
import static org.wpilib.math.optimization.Constraints.le;

import org.wpilib.math.autodiff.Variable;
import org.wpilib.math.autodiff.VariableMatrix;

/**
 * This class computes the optimal current allocation for a list of subsystems given a list of their
 * desired currents and current tolerances that determine which subsystem gets less current if the
 * current budget is exceeded. Subsystems with a smaller tolerance are given higher priority.
 */
public class CurrentManager implements AutoCloseable {
  private final Problem m_problem = new Problem();
  private final VariableMatrix m_desiredCurrents;
  private final VariableMatrix m_allocatedCurrents;

  /**
   * Constructs a CurrentManager.
   *
   * @param currentTolerances The relative current tolerance of each subsystem.
   * @param maxCurrent The current budget to allocate between subsystems.
   */
  public CurrentManager(double[] currentTolerances, double maxCurrent) {
    this.m_desiredCurrents = new VariableMatrix(currentTolerances.length, 1);
    this.m_allocatedCurrents = m_problem.decisionVariable(currentTolerances.length);

    // Ensure m_desired_currents contains initialized Variables
    for (int row = 0; row < m_desiredCurrents.rows(); ++row) {
      // Don't initialize to 0 or 1, because those will get folded by Sleipnir
      m_desiredCurrents.get(row).setValue(Double.POSITIVE_INFINITY);
    }

    var J = new Variable(0.0);
    var currentSum = new Variable(0.0);
    for (int i = 0; i < currentTolerances.length; ++i) {
      // The weight is 1/tolᵢ² where tolᵢ is the tolerance between the desired
      // and allocated current for subsystem i
      var error = m_desiredCurrents.get(i).minus(m_allocatedCurrents.get(i));
      J = J.plus(error.times(error).div(currentTolerances[i] * currentTolerances[i]));

      currentSum = currentSum.plus(m_allocatedCurrents.get(i));

      // Currents must be nonnegative
      m_problem.subjectTo(ge(m_allocatedCurrents.get(i), 0.0));
    }
    m_problem.minimize(J);

    // Keep total current below maximum
    m_problem.subjectTo(le(currentSum, maxCurrent));
  }

  @Override
  public void close() {
    m_problem.close();
  }

  /**
   * Returns the optimal current allocation for a list of subsystems given a list of their desired
   * currents and current tolerances that determine which subsystem gets less current if the current
   * budget is exceeded. Subsystems with a smaller tolerance are given higher priority.
   *
   * @param desiredCurrents The desired current for each subsystem.
   * @throws RuntimeException if the number of desired currents doesn't equal the number of
   *     tolerances passed in the constructor.
   */
  public double[] calculate(double[] desiredCurrents) {
    if (m_desiredCurrents.rows() != desiredCurrents.length) {
      throw new RuntimeException(
          "Number of desired currents must equal the number of tolerances passed in the "
              + "constructor.");
    }

    for (int i = 0; i < desiredCurrents.length; ++i) {
      m_desiredCurrents.get(i).setValue(desiredCurrents[i]);
    }

    m_problem.solve();

    var result = new double[desiredCurrents.length];
    for (int i = 0; i < desiredCurrents.length; ++i) {
      result[i] = Math.max(m_allocatedCurrents.value(i), 0.0);
    }

    return result;
  }
}
