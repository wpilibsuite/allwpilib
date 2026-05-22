// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.optimization;

import java.util.ArrayList;
import org.ejml.simple.SimpleMatrix;
import org.wpilib.math.autodiff.Variable;
import org.wpilib.math.autodiff.VariableBlock;
import org.wpilib.math.autodiff.VariableMatrix;

/** Constraint creation helper functions. */
public final class Constraints {
  /** Utility class. */
  private Constraints() {}

  // ==

  /**
   * Equality operator that returns an equality constraint for a double and a Variable.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static EqualityConstraints eq(double lhs, Variable rhs) {
    return eq(new Variable(lhs), rhs);
  }

  /**
   * Equality operator that returns an equality constraint for a Variable and a double.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static EqualityConstraints eq(Variable lhs, double rhs) {
    return eq(lhs, new Variable(rhs));
  }

  /**
   * Equality operator that returns an equality constraint for two Variables.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static EqualityConstraints eq(Variable lhs, Variable rhs) {
    return new EqualityConstraints(new Variable[] {lhs.minus(rhs)});
  }

  /**
   * Equality operator that returns an equality constraint for a double and a VariableBlock.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static EqualityConstraints eq(double lhs, VariableBlock rhs) {
    return eq(new Variable(lhs), new VariableMatrix(rhs));
  }

  /**
   * Equality operator that returns an equality constraint for a Variable and a VariableBlock.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static EqualityConstraints eq(Variable lhs, VariableBlock rhs) {
    return eq(lhs, new VariableMatrix(rhs));
  }

  /**
   * Equality operator that returns an equality constraint for a double and a VariableMatrix.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static EqualityConstraints eq(double lhs, VariableMatrix rhs) {
    return eq(new Variable(lhs), rhs);
  }

  /**
   * Equality operator that returns an equality constraint for a Variable and a VariableMatrix.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static EqualityConstraints eq(Variable lhs, VariableMatrix rhs) {
    var constraints = new ArrayList<Variable>(rhs.rows() * rhs.cols());
    for (int row = 0; row < rhs.rows(); ++row) {
      for (int col = 0; col < rhs.cols(); ++col) {
        constraints.add(lhs.minus(rhs.get(row, col)));
      }
    }

    var array = new Variable[constraints.size()];
    return new EqualityConstraints(constraints.toArray(array));
  }

  /**
   * Equality operator that returns an equality constraint for a VariableBlock and a double.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static EqualityConstraints eq(VariableBlock lhs, double rhs) {
    return eq(new VariableMatrix(lhs), new Variable(rhs));
  }

  /**
   * Equality operator that returns an equality constraint for a VariableBlock and a Variable.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static EqualityConstraints eq(VariableBlock lhs, Variable rhs) {
    return eq(new VariableMatrix(lhs), rhs);
  }

  /**
   * Equality operator that returns an equality constraint for a VariableMatrix and a double.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static EqualityConstraints eq(VariableMatrix lhs, double rhs) {
    return eq(lhs, new Variable(rhs));
  }

  /**
   * Equality operator that returns an equality constraint for a VariableMatrix and a Variable.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static EqualityConstraints eq(VariableMatrix lhs, Variable rhs) {
    var constraints = new ArrayList<Variable>(lhs.rows() * lhs.cols());
    for (int row = 0; row < lhs.rows(); ++row) {
      for (int col = 0; col < lhs.cols(); ++col) {
        constraints.add(lhs.get(row, col).minus(rhs));
      }
    }

    var array = new Variable[constraints.size()];
    return new EqualityConstraints(constraints.toArray(array));
  }

  /**
   * Equality operator that returns an equality constraint for two VariableBlocks.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static EqualityConstraints eq(VariableBlock lhs, VariableBlock rhs) {
    return eq(new VariableMatrix(lhs), new VariableMatrix(rhs));
  }

  /**
   * Equality operator that returns an equality constraint for a VariableBlock and a VariableMatrix.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static EqualityConstraints eq(VariableBlock lhs, VariableMatrix rhs) {
    return eq(new VariableMatrix(lhs), rhs);
  }

  /**
   * Equality operator that returns an equality constraint for a VariableMatrix and a VariableBlock.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static EqualityConstraints eq(VariableMatrix lhs, VariableBlock rhs) {
    return eq(lhs, new VariableMatrix(rhs));
  }

  /**
   * Equality operator that returns an equality constraint for a double[][] and a VariableBlock.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static EqualityConstraints eq(double[][] lhs, VariableBlock rhs) {
    return eq(new VariableMatrix(lhs), new VariableMatrix(rhs));
  }

  /**
   * Equality operator that returns an equality constraint for a SimpleMatrix and a VariableBlock.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static EqualityConstraints eq(SimpleMatrix lhs, VariableBlock rhs) {
    return eq(new VariableMatrix(lhs), new VariableMatrix(rhs));
  }

  /**
   * Equality operator that returns an equality constraint for a double array and a VariableMatrix.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static EqualityConstraints eq(double[][] lhs, VariableMatrix rhs) {
    return eq(new VariableMatrix(lhs), rhs);
  }

  /**
   * Equality operator that returns an equality constraint for a SimpleMatrix and a VariableMatrix.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static EqualityConstraints eq(SimpleMatrix lhs, VariableMatrix rhs) {
    return eq(new VariableMatrix(lhs), rhs);
  }

  /**
   * Equality operator that returns an equality constraint for a VariableBlock and a double array.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static EqualityConstraints eq(VariableBlock lhs, double[][] rhs) {
    return eq(new VariableMatrix(lhs), new VariableMatrix(rhs));
  }

  /**
   * Equality operator that returns an equality constraint for a VariableBlock and a SimpleMatrix.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static EqualityConstraints eq(VariableBlock lhs, SimpleMatrix rhs) {
    return eq(new VariableMatrix(lhs), new VariableMatrix(rhs));
  }

  /**
   * Equality operator that returns an equality constraint for a VariableMatrix and a double array.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static EqualityConstraints eq(VariableMatrix lhs, double[][] rhs) {
    return eq(lhs, new VariableMatrix(rhs));
  }

  /**
   * Equality operator that returns an equality constraint for a VariableMatrix and a SimpleMatrix.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static EqualityConstraints eq(VariableMatrix lhs, SimpleMatrix rhs) {
    return eq(lhs, new VariableMatrix(rhs));
  }

  /**
   * Equality operator that returns an equality constraint for two VariableMatrices.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static EqualityConstraints eq(VariableMatrix lhs, VariableMatrix rhs) {
    assert lhs.rows() == rhs.rows() && lhs.cols() == rhs.cols();

    var constraints = new ArrayList<Variable>(lhs.rows() * lhs.cols());
    for (int row = 0; row < lhs.rows(); ++row) {
      for (int col = 0; col < lhs.cols(); ++col) {
        constraints.add(lhs.get(row, col).minus(rhs.get(row, col)));
      }
    }

    var array = new Variable[constraints.size()];
    return new EqualityConstraints(constraints.toArray(array));
  }

  // <

  /**
   * Less-than comparison operator that returns an inequality constraint for a double and a
   * Variable.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints lt(double lhs, Variable rhs) {
    return ge(rhs, new Variable(lhs));
  }

  /**
   * Less-than comparison operator that returns an inequality constraint for a Variable and a
   * double.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints lt(Variable lhs, double rhs) {
    return ge(new Variable(rhs), lhs);
  }

  /**
   * Less-than comparison operator that returns an inequality constraint for two Variables.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints lt(Variable lhs, Variable rhs) {
    return ge(rhs, lhs);
  }

  /**
   * Less-than comparison operator that returns an inequality constraint for a double and a
   * VariableBlock.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints lt(double lhs, VariableBlock rhs) {
    return ge(new VariableMatrix(rhs), new Variable(lhs));
  }

  /**
   * Less-than comparison operator that returns an inequality constraint for a Variable and a
   * VariableBlock.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints lt(Variable lhs, VariableBlock rhs) {
    return ge(new VariableMatrix(rhs), lhs);
  }

  /**
   * Less-than comparison operator that returns an inequality constraint for a double and a
   * VariableMatrix.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints lt(double lhs, VariableMatrix rhs) {
    return ge(rhs, new Variable(lhs));
  }

  /**
   * Less-than comparison operator that returns an inequality constraint for a Variable and a
   * VariableMatrix.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints lt(Variable lhs, VariableMatrix rhs) {
    return ge(rhs, lhs);
  }

  /**
   * Less-than comparison operator that returns an inequality constraint for a VariableBlock and a
   * double.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints lt(VariableBlock lhs, double rhs) {
    return ge(new Variable(rhs), new VariableMatrix(lhs));
  }

  /**
   * Less-than comparison operator that returns an inequality constraint for a VariableBlock and a
   * Variable.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints lt(VariableBlock lhs, Variable rhs) {
    return ge(rhs, new VariableMatrix(lhs));
  }

  /**
   * Less-than comparison operator that returns an inequality constraint for a VariableMatrix and a
   * double.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints lt(VariableMatrix lhs, double rhs) {
    return ge(new Variable(rhs), lhs);
  }

  /**
   * Less-than comparison operator that returns an inequality constraint for a VariableMatrix and a
   * Variable.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints lt(VariableMatrix lhs, Variable rhs) {
    return ge(rhs, lhs);
  }

  /**
   * Less-than comparison operator that returns an inequality constraint for two VariableBlocks.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints lt(VariableBlock lhs, VariableBlock rhs) {
    return ge(new VariableMatrix(rhs), new VariableMatrix(lhs));
  }

  /**
   * Less-than comparison operator that returns an inequality constraint for a VariableBlock and a
   * VariableMatrix.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints lt(VariableBlock lhs, VariableMatrix rhs) {
    return ge(rhs, new VariableMatrix(lhs));
  }

  /**
   * Less-than comparison operator that returns an inequality constraint for a VariableMatrix and a
   * VariableBlock.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints lt(VariableMatrix lhs, VariableBlock rhs) {
    return ge(new VariableMatrix(rhs), lhs);
  }

  /**
   * Less-than comparison operator that returns an inequality constraint for a double array and a
   * VariableBlock.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static InequalityConstraints lt(double[][] lhs, VariableBlock rhs) {
    return lt(new VariableMatrix(lhs), new VariableMatrix(rhs));
  }

  /**
   * Less-than comparison operator that returns an inequality constraint for a SimpleMatrix and a
   * VariableBlock.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static InequalityConstraints lt(SimpleMatrix lhs, VariableBlock rhs) {
    return lt(new VariableMatrix(lhs), new VariableMatrix(rhs));
  }

  /**
   * Less-than comparison operator that returns an inequality constraint for a double array and a
   * VariableMatrix.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static InequalityConstraints lt(double[][] lhs, VariableMatrix rhs) {
    return lt(new VariableMatrix(lhs), rhs);
  }

  /**
   * Less-than comparison operator that returns an inequality constraint for a SimpleMatrix and a
   * VariableMatrix.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static InequalityConstraints lt(SimpleMatrix lhs, VariableMatrix rhs) {
    return lt(new VariableMatrix(lhs), rhs);
  }

  /**
   * Less-than comparison operator that returns an inequality constraint for a VariableBlock and a
   * double array.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static InequalityConstraints lt(VariableBlock lhs, double[][] rhs) {
    return lt(new VariableMatrix(lhs), new VariableMatrix(rhs));
  }

  /**
   * Less-than comparison operator that returns an inequality constraint for a VariableBlock and a
   * SimpleMatrix.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static InequalityConstraints lt(VariableBlock lhs, SimpleMatrix rhs) {
    return lt(new VariableMatrix(lhs), new VariableMatrix(rhs));
  }

  /**
   * Less-than comparison operator that returns an inequality constraint for a VariableMatrix and a
   * double array.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static InequalityConstraints lt(VariableMatrix lhs, double[][] rhs) {
    return lt(lhs, new VariableMatrix(rhs));
  }

  /**
   * Less-than comparison operator that returns an inequality constraint for a VariableMatrix and a
   * SimpleMatrix.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static InequalityConstraints lt(VariableMatrix lhs, SimpleMatrix rhs) {
    return lt(lhs, new VariableMatrix(rhs));
  }

  /**
   * Less-than comparison operator that returns an inequality constraint for two VariableMatrices.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints lt(VariableMatrix lhs, VariableMatrix rhs) {
    return ge(rhs, lhs);
  }

  // <=

  /**
   * Less-than-or-equal-to comparison operator that returns an inequality constraint for a double
   * and a Variable.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints le(double lhs, Variable rhs) {
    return ge(rhs, new Variable(lhs));
  }

  /**
   * Less-than-or-equal-to comparison operator that returns an inequality constraint for a Variable
   * and a double.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints le(Variable lhs, double rhs) {
    return ge(new Variable(rhs), lhs);
  }

  /**
   * Less-than-or-equal-to comparison operator that returns an inequality constraint for two
   * Variables.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints le(Variable lhs, Variable rhs) {
    return ge(rhs, lhs);
  }

  /**
   * Less-than-or-equal-to comparison operator that returns an inequality constraint for a double
   * and a VariableBlock.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints le(double lhs, VariableBlock rhs) {
    return ge(new VariableMatrix(rhs), new Variable(lhs));
  }

  /**
   * Less-than-or-equal-to comparison operator that returns an inequality constraint for a Variable
   * and a VariableBlock.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints le(Variable lhs, VariableBlock rhs) {
    return ge(new VariableMatrix(rhs), lhs);
  }

  /**
   * Less-than-or-equal-to comparison operator that returns an inequality constraint for a double
   * and a VariableMatrix.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints le(double lhs, VariableMatrix rhs) {
    return ge(rhs, new Variable(lhs));
  }

  /**
   * Less-than-or-equal-to comparison operator that returns an inequality constraint for a Variable
   * and a VariableMatrix.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints le(Variable lhs, VariableMatrix rhs) {
    return ge(rhs, lhs);
  }

  /**
   * Less-than-or-equal-to comparison operator that returns an inequality constraint for a
   * VariableBlock and a double.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints le(VariableBlock lhs, double rhs) {
    return ge(new Variable(rhs), new VariableMatrix(lhs));
  }

  /**
   * Less-than-or-equal-to comparison operator that returns an inequality constraint for a
   * VariableBlock and a Variable.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints le(VariableBlock lhs, Variable rhs) {
    return ge(rhs, new VariableMatrix(lhs));
  }

  /**
   * Less-than-or-equal-to comparison operator that returns an inequality constraint for a
   * VariableMatrix and a double.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints le(VariableMatrix lhs, double rhs) {
    return ge(new Variable(rhs), lhs);
  }

  /**
   * Less-than-or-equal-to comparison operator that returns an inequality constraint for a
   * VariableMatrix and a Variable.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints le(VariableMatrix lhs, Variable rhs) {
    return ge(rhs, lhs);
  }

  /**
   * Less-than-or-equal-to comparison operator that returns an inequality constraint for two
   * VariableBlocks.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints le(VariableBlock lhs, VariableBlock rhs) {
    return ge(new VariableMatrix(rhs), new VariableMatrix(lhs));
  }

  /**
   * Less-than-or-equal-to comparison operator that returns an inequality constraint for a
   * VariableBlock and a VariableMatrix.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints le(VariableBlock lhs, VariableMatrix rhs) {
    return ge(rhs, new VariableMatrix(lhs));
  }

  /**
   * Less-than-or-equal-to comparison operator that returns an inequality constraint for a
   * VariableMatrix and a VariableBlock.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints le(VariableMatrix lhs, VariableBlock rhs) {
    return ge(new VariableMatrix(rhs), lhs);
  }

  /**
   * Less-than-or-equal-to comparison operator that returns an inequality constraint for a double
   * array and a VariableBlock.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static InequalityConstraints le(double[][] lhs, VariableBlock rhs) {
    return le(new VariableMatrix(lhs), new VariableMatrix(rhs));
  }

  /**
   * Less-than-or-equal-to comparison operator that returns an inequality constraint for a
   * SimpleMatrix and a VariableBlock.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static InequalityConstraints le(SimpleMatrix lhs, VariableBlock rhs) {
    return le(new VariableMatrix(lhs), new VariableMatrix(rhs));
  }

  /**
   * Less-than-or-equal-to comparison operator that returns an inequality constraint for a double
   * array and a VariableMatrix.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static InequalityConstraints le(double[][] lhs, VariableMatrix rhs) {
    return le(new VariableMatrix(lhs), rhs);
  }

  /**
   * Less-than-or-equal-to comparison operator that returns an inequality constraint for a
   * SimpleMatrix and a VariableMatrix.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static InequalityConstraints le(SimpleMatrix lhs, VariableMatrix rhs) {
    return le(new VariableMatrix(lhs), rhs);
  }

  /**
   * Less-than-or-equal-to comparison operator that returns an inequality constraint for a
   * VariableBlock and a double array.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static InequalityConstraints le(VariableBlock lhs, double[][] rhs) {
    return le(new VariableMatrix(lhs), new VariableMatrix(rhs));
  }

  /**
   * Less-than-or-equal-to comparison operator that returns an inequality constraint for a
   * VariableBlock and a SimpleMatrix.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static InequalityConstraints le(VariableBlock lhs, SimpleMatrix rhs) {
    return le(new VariableMatrix(lhs), new VariableMatrix(rhs));
  }

  /**
   * Less-than-or-equal-to comparison operator that returns an inequality constraint for a
   * VariableMatrix and a double array.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static InequalityConstraints le(VariableMatrix lhs, double[][] rhs) {
    return le(lhs, new VariableMatrix(rhs));
  }

  /**
   * Less-than-or-equal-to comparison operator that returns an inequality constraint for a
   * VariableMatrix and a SimpleMatrix.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static InequalityConstraints le(VariableMatrix lhs, SimpleMatrix rhs) {
    return le(lhs, new VariableMatrix(rhs));
  }

  /**
   * Less-than-or-equal-to comparison operator that returns an inequality constraint for two
   * VariableMatrices.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints le(VariableMatrix lhs, VariableMatrix rhs) {
    return ge(rhs, lhs);
  }

  // >

  /**
   * Greater-than comparison operator that returns an inequality constraint for a double and a
   * Variable.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints gt(double lhs, Variable rhs) {
    return ge(new Variable(lhs), rhs);
  }

  /**
   * Greater-than comparison operator that returns an inequality constraint for a Variable and a
   * double.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints gt(Variable lhs, double rhs) {
    return ge(lhs, new Variable(rhs));
  }

  /**
   * Greater-than comparison operator that returns an inequality constraint for two Variables.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints gt(Variable lhs, Variable rhs) {
    return ge(lhs, rhs);
  }

  /**
   * Greater-than comparison operator that returns an inequality constraint for a double and a
   * VariableBlock.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints gt(double lhs, VariableBlock rhs) {
    return ge(new Variable(lhs), new VariableMatrix(rhs));
  }

  /**
   * Greater-than comparison operator that returns an inequality constraint for a Variable and a
   * VariableBlock.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints gt(Variable lhs, VariableBlock rhs) {
    return ge(lhs, new VariableMatrix(rhs));
  }

  /**
   * Greater-than comparison operator that returns an inequality constraint for a double and a
   * VariableMatrix.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints gt(double lhs, VariableMatrix rhs) {
    return ge(new Variable(lhs), rhs);
  }

  /**
   * Greater-than comparison operator that returns an inequality constraint for a Variable and a
   * VariableMatrix.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints gt(Variable lhs, VariableMatrix rhs) {
    return ge(lhs, rhs);
  }

  /**
   * Greater-than comparison operator that returns an inequality constraint for a VariableBlock and
   * a double.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints gt(VariableBlock lhs, double rhs) {
    return ge(new VariableMatrix(lhs), new Variable(rhs));
  }

  /**
   * Greater-than comparison operator that returns an inequality constraint for a VariableBlock and
   * a Variable.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints gt(VariableBlock lhs, Variable rhs) {
    return ge(new VariableMatrix(lhs), rhs);
  }

  /**
   * Greater-than comparison operator that returns an inequality constraint for a VariableMatrix and
   * a double.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints gt(VariableMatrix lhs, double rhs) {
    return ge(lhs, new Variable(rhs));
  }

  /**
   * Greater-than comparison operator that returns an inequality constraint for a VariableMatrix and
   * a Variable.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints gt(VariableMatrix lhs, Variable rhs) {
    return ge(lhs, rhs);
  }

  /**
   * Greater-than comparison operator that returns an inequality constraint for two VariableBlocks.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints gt(VariableBlock lhs, VariableBlock rhs) {
    return ge(new VariableMatrix(lhs), new VariableMatrix(rhs));
  }

  /**
   * Greater-than comparison operator that returns an inequality constraint for a VariableBlock and
   * a VariableMatrix.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints gt(VariableBlock lhs, VariableMatrix rhs) {
    return ge(new VariableMatrix(lhs), rhs);
  }

  /**
   * Greater-than comparison operator that returns an inequality constraint for a VariableMatrix and
   * a VariableBlock.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints gt(VariableMatrix lhs, VariableBlock rhs) {
    return ge(lhs, new VariableMatrix(rhs));
  }

  /**
   * Greater-than comparison operator that returns an inequality constraint for a double array and a
   * VariableBlock.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static InequalityConstraints gt(double[][] lhs, VariableBlock rhs) {
    return gt(new VariableMatrix(lhs), new VariableMatrix(rhs));
  }

  /**
   * Greater-than comparison operator that returns an inequality constraint for a SimpleMatrix and a
   * VariableBlock.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static InequalityConstraints gt(SimpleMatrix lhs, VariableBlock rhs) {
    return gt(new VariableMatrix(lhs), new VariableMatrix(rhs));
  }

  /**
   * Greater-than comparison operator that returns an inequality constraint for a double array and a
   * VariableMatrix.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static InequalityConstraints gt(double[][] lhs, VariableMatrix rhs) {
    return gt(new VariableMatrix(lhs), rhs);
  }

  /**
   * Greater-than comparison operator that returns an inequality constraint for a SimpleMatrix and a
   * VariableMatrix.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static InequalityConstraints gt(SimpleMatrix lhs, VariableMatrix rhs) {
    return gt(new VariableMatrix(lhs), rhs);
  }

  /**
   * Greater-than comparison operator that returns an inequality constraint for a VariableBlock and
   * a double array.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static InequalityConstraints gt(VariableBlock lhs, double[][] rhs) {
    return gt(new VariableMatrix(lhs), new VariableMatrix(rhs));
  }

  /**
   * Greater-than comparison operator that returns an inequality constraint for a VariableBlock and
   * a SimpleMatrix.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static InequalityConstraints gt(VariableBlock lhs, SimpleMatrix rhs) {
    return gt(new VariableMatrix(lhs), new VariableMatrix(rhs));
  }

  /**
   * Greater-than comparison operator that returns an inequality constraint for a VariableMatrix and
   * a double array.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static InequalityConstraints gt(VariableMatrix lhs, double[][] rhs) {
    return gt(lhs, new VariableMatrix(rhs));
  }

  /**
   * Greater-than comparison operator that returns an inequality constraint for a VariableMatrix and
   * a SimpleMatrix.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static InequalityConstraints gt(VariableMatrix lhs, SimpleMatrix rhs) {
    return gt(lhs, new VariableMatrix(rhs));
  }

  /**
   * Greater-than comparison operator that returns an inequality constraint for two
   * VariableMatrices.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints gt(VariableMatrix lhs, VariableMatrix rhs) {
    return ge(lhs, rhs);
  }

  // >=

  /**
   * Greater-than-or-equal-to comparison operator that returns an inequality constraint for a double
   * and a Variable.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints ge(double lhs, Variable rhs) {
    return ge(new Variable(lhs), rhs);
  }

  /**
   * Greater-than-or-equal-to comparison operator that returns an inequality constraint for a
   * Variable and a double.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints ge(Variable lhs, double rhs) {
    return ge(lhs, new Variable(rhs));
  }

  /**
   * Greater-than-or-equal-to comparison operator that returns an inequality constraint for two
   * Variables.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints ge(Variable lhs, Variable rhs) {
    return new InequalityConstraints(new Variable[] {lhs.minus(rhs)});
  }

  /**
   * Greater-than-or-equal-to comparison operator that returns an inequality constraint for a double
   * and a VariableBlock.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints ge(double lhs, VariableBlock rhs) {
    return ge(new Variable(lhs), new VariableMatrix(rhs));
  }

  /**
   * Greater-than-or-equal-to comparison operator that returns an inequality constraint for a
   * Variable and a VariableBlock.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints ge(Variable lhs, VariableBlock rhs) {
    return ge(lhs, new VariableMatrix(rhs));
  }

  /**
   * Greater-than-or-equal-to comparison operator that returns an inequality constraint for a double
   * and a VariableMatrix.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints ge(double lhs, VariableMatrix rhs) {
    return ge(new Variable(lhs), rhs);
  }

  /**
   * Greater-than-or-equal-to comparison operator that returns an inequality constraint for a
   * Variable and a VariableMatrix.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints ge(Variable lhs, VariableMatrix rhs) {
    var constraints = new ArrayList<Variable>(rhs.rows() * rhs.cols());
    for (int row = 0; row < rhs.rows(); ++row) {
      for (int col = 0; col < rhs.cols(); ++col) {
        constraints.add(lhs.minus(rhs.get(row, col)));
      }
    }

    var array = new Variable[constraints.size()];
    return new InequalityConstraints(constraints.toArray(array));
  }

  /**
   * Greater-than-or-equal-to comparison operator that returns an inequality constraint for a
   * VariableBlock and a double.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints ge(VariableBlock lhs, double rhs) {
    return ge(new VariableMatrix(lhs), new Variable(rhs));
  }

  /**
   * Greater-than-or-equal-to comparison operator that returns an inequality constraint for a
   * VariableBlock and a Variable.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints ge(VariableBlock lhs, Variable rhs) {
    return ge(new VariableMatrix(lhs), rhs);
  }

  /**
   * Greater-than-or-equal-to comparison operator that returns an inequality constraint for a
   * VariableMatrix and a double.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints ge(VariableMatrix lhs, double rhs) {
    return ge(lhs, new Variable(rhs));
  }

  /**
   * Greater-than-or-equal-to comparison operator that returns an inequality constraint for a
   * VariableMatrix and a Variable.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints ge(VariableMatrix lhs, Variable rhs) {
    var constraints = new ArrayList<Variable>(lhs.rows() * lhs.cols());
    for (int row = 0; row < lhs.rows(); ++row) {
      for (int col = 0; col < lhs.cols(); ++col) {
        constraints.add(lhs.get(row, col).minus(rhs));
      }
    }

    var array = new Variable[constraints.size()];
    return new InequalityConstraints(constraints.toArray(array));
  }

  /**
   * Greater-than-or-equal-to comparison operator that returns an inequality constraint for two
   * VariableBlocks.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints ge(VariableBlock lhs, VariableBlock rhs) {
    return ge(new VariableMatrix(lhs), new VariableMatrix(rhs));
  }

  /**
   * Greater-than-or-equal-to comparison operator that returns an inequality constraint for a
   * VariableBlock and a VariableMatrix.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints ge(VariableBlock lhs, VariableMatrix rhs) {
    return ge(new VariableMatrix(lhs), rhs);
  }

  /**
   * Greater-than-or-equal-to comparison operator that returns an inequality constraint for a
   * VariableMatrix and a VariableBlock.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints ge(VariableMatrix lhs, VariableBlock rhs) {
    return ge(lhs, new VariableMatrix(rhs));
  }

  /**
   * Greater-than-or-equal-to comparison operator that returns an inequality constraint for a double
   * array and a VariableBlock.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static InequalityConstraints ge(double[][] lhs, VariableBlock rhs) {
    return ge(new VariableMatrix(lhs), new VariableMatrix(rhs));
  }

  /**
   * Greater-than-or-equal-to comparison operator that returns an inequality constraint for a
   * SimpleMatrix and a VariableBlock.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static InequalityConstraints ge(SimpleMatrix lhs, VariableBlock rhs) {
    return ge(new VariableMatrix(lhs), new VariableMatrix(rhs));
  }

  /**
   * Greater-than-or-equal-to comparison operator that returns an inequality constraint for a double
   * array and a VariableMatrix.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static InequalityConstraints ge(double[][] lhs, VariableMatrix rhs) {
    return ge(new VariableMatrix(lhs), rhs);
  }

  /**
   * Greater-than-or-equal-to comparison operator that returns an inequality constraint for a
   * SimpleMatrix and a VariableMatrix.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static InequalityConstraints ge(SimpleMatrix lhs, VariableMatrix rhs) {
    return ge(new VariableMatrix(lhs), rhs);
  }

  /**
   * Greater-than-or-equal-to comparison operator that returns an inequality constraint for a
   * VariableBlock and a double array.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static InequalityConstraints ge(VariableBlock lhs, double[][] rhs) {
    return ge(new VariableMatrix(lhs), new VariableMatrix(rhs));
  }

  /**
   * Greater-than-or-equal-to comparison operator that returns an inequality constraint for a
   * VariableBlock and a SimpleMatrix.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static InequalityConstraints ge(VariableBlock lhs, SimpleMatrix rhs) {
    return ge(new VariableMatrix(lhs), new VariableMatrix(rhs));
  }

  /**
   * Greater-than-or-equal-to comparison operator that returns an inequality constraint for a
   * VariableMatrix and a double array.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static InequalityConstraints ge(VariableMatrix lhs, double[][] rhs) {
    return ge(lhs, new VariableMatrix(rhs));
  }

  /**
   * Greater-than-or-equal-to comparison operator that returns an inequality constraint for a
   * VariableMatrix and a SimpleMatrix.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Equality constraints.
   */
  public static InequalityConstraints ge(VariableMatrix lhs, SimpleMatrix rhs) {
    return ge(lhs, new VariableMatrix(rhs));
  }

  /**
   * Greater-than-or-equal-to comparison operator that returns an inequality constraint for two
   * VariableMatrices.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   * @return Inequality constraints.
   */
  public static InequalityConstraints ge(VariableMatrix lhs, VariableMatrix rhs) {
    assert lhs.rows() == rhs.rows() && lhs.cols() == rhs.cols();

    var constraints = new ArrayList<Variable>(lhs.rows() * lhs.cols());
    for (int row = 0; row < lhs.rows(); ++row) {
      for (int col = 0; col < lhs.cols(); ++col) {
        constraints.add(lhs.get(row, col).minus(rhs.get(row, col)));
      }
    }

    var array = new Variable[constraints.size()];
    return new InequalityConstraints(constraints.toArray(array));
  }

  /**
   * Helper function for creating bound constraints.
   *
   * @param l Lower bound.
   * @param x Variable to bound.
   * @param u Upper bound.
   * @return Inequality constraints.
   */
  public static InequalityConstraints bounds(double l, Variable x, double u) {
    return bounds(l, new VariableMatrix(x), u);
  }

  /**
   * Helper function for creating bound constraints.
   *
   * @param l Lower bound.
   * @param x Variable to bound.
   * @param u Upper bound.
   * @return Inequality constraints.
   */
  public static InequalityConstraints bounds(double l, VariableMatrix x, double u) {
    var ineq1 = le(l, x).constraints;
    var ineq2 = le(x, u).constraints;
    var result = new Variable[ineq1.length + ineq2.length];
    System.arraycopy(ineq1, 0, result, 0, ineq1.length);
    System.arraycopy(ineq2, 0, result, ineq1.length, ineq2.length);
    return new InequalityConstraints(result);
  }

  /**
   * Helper function for creating bound constraints.
   *
   * @param l Lower bound.
   * @param x Variable to bound.
   * @param u Upper bound.
   * @return Inequality constraints.
   */
  public static InequalityConstraints bounds(double l, VariableBlock x, double u) {
    return bounds(l, new VariableMatrix(x), u);
  }
}
