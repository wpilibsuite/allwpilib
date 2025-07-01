// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.autodiff;

/** An autodiff variable pointing to an expression node. */
public class Variable implements AutoCloseable {
  private long m_handle;

  /** Constructs a linear Variable with a value of zero. */
  public Variable() {
    m_handle = VariableJNI.createDefault();
  }

  /**
   * Constructs a Variable from a floating point type.
   *
   * @param value The value of the Variable.
   */
  public Variable(double value) {
    m_handle = VariableJNI.createDouble(value);
  }

  /**
   * Constructs a Variable from an integral type.
   *
   * @param value The value of the Variable.
   */
  public Variable(int value) {
    m_handle = VariableJNI.createInt(value);
  }

  @Override
  public void close() {
    if (m_handle != 0) {
      VariableJNI.destroy(m_handle);
      m_handle = 0;
    }
  }

  /**
   * Sets Variable's internal value.
   *
   * @param value The value of the Variable.
   */
  public void setValue(double value) {
    VariableJNI.setValue(m_handle, value);
  }

  /**
   * Variable-Variable multiplication operator.
   *
   * @param rhs Operator right-hand side.
   * @return Result of multiplication.
   */
  public Variable times(Variable rhs) {
    return fromHandle(VariableJNI.times(m_handle, rhs.getHandle()));
  }

  /**
   * Variable-Variable multiplication operator.
   *
   * @param rhs Operator right-hand side.
   * @return Result of multiplication.
   */
  public Variable times(double rhs) {
    return times(new Variable(rhs));
  }

  /**
   * Variable-Variable division operator.
   *
   * @param rhs Operator right-hand side.
   * @return Result of division.
   */
  public Variable div(Variable rhs) {
    return fromHandle(VariableJNI.div(m_handle, rhs.getHandle()));
  }

  /**
   * Variable-Variable division operator.
   *
   * @param rhs Operator right-hand side.
   * @return Result of division.
   */
  public Variable div(double rhs) {
    return div(new Variable(rhs));
  }

  /**
   * Variable-Variable addition operator.
   *
   * @param rhs Operator right-hand side.
   * @return Result of addition.
   */
  public Variable plus(Variable rhs) {
    return fromHandle(VariableJNI.plus(m_handle, rhs.getHandle()));
  }

  /**
   * Variable-Variable addition operator.
   *
   * @param rhs Operator right-hand side.
   * @return Result of addition.
   */
  public Variable plus(double rhs) {
    return plus(new Variable(rhs));
  }

  /**
   * Variable-Variable subtraction operator.
   *
   * @param rhs Operator right-hand side.
   * @return Result of subtraction.
   */
  public Variable minus(Variable rhs) {
    return fromHandle(VariableJNI.minus(m_handle, rhs.getHandle()));
  }

  /**
   * Variable-Variable subtraction operator.
   *
   * @param rhs Operator right-hand side.
   * @return Result of subtraction.
   */
  public Variable minus(double rhs) {
    return fromHandle(VariableJNI.minus(m_handle, new Variable(rhs).getHandle()));
  }

  /**
   * Unary minus operator.
   *
   * @return Result of unary minus.
   */
  public Variable unaryMinus() {
    return fromHandle(VariableJNI.unaryMinus(m_handle));
  }

  /**
   * Unary plus operator.
   *
   * @return Result of unary plus.
   */
  public Variable unaryPlus() {
    return this;
  }

  /**
   * Returns the value of this variable.
   *
   * @return The value of this variable.
   */
  public double value() {
    return VariableJNI.value(m_handle);
  }

  /**
   * Returns the type of this expression (constant, linear, quadratic, or nonlinear).
   *
   * @return The type of this expression.
   */
  public ExpressionType type() {
    return ExpressionType.fromInteger(VariableJNI.type(m_handle));
  }

  /**
   * Returns internal handle.
   *
   * @return Internal handle.
   */
  public long getHandle() {
    return m_handle;
  }

  /**
   * Constructs a Variable from an internal handle.
   *
   * <p>This function is for JNI only.
   *
   * @param handle Variable handle.
   * @return Variable wrapping handle.
   */
  public static Variable fromHandle(long handle) {
    var result = new Variable();
    result.m_handle = handle;
    return result;
  }

  /**
   * Math.abs() for Variables.
   *
   * @param x The argument.
   * @return Result of abs().
   */
  public static Variable abs(Variable x) {
    return fromHandle(VariableJNI.abs(x.getHandle()));
  }

  /**
   * Math.acos() for Variables.
   *
   * @param x The argument.
   * @return Result of acos().
   */
  public static Variable acos(Variable x) {
    return fromHandle(VariableJNI.acos(x.getHandle()));
  }

  /**
   * Math.asin() for Variables.
   *
   * @param x The argument.
   * @return Result of asin().
   */
  public static Variable asin(Variable x) {
    return fromHandle(VariableJNI.asin(x.getHandle()));
  }

  /**
   * Math.atan() for Variables.
   *
   * @param x The argument.
   * @return Result of atan().
   */
  public static Variable atan(Variable x) {
    return fromHandle(VariableJNI.atan(x.getHandle()));
  }

  /**
   * Math.atan2() for Variables.
   *
   * @param y The y argument.
   * @param x The x argument.
   * @return Result of atan2().
   */
  public static Variable atan2(double y, Variable x) {
    return atan2(new Variable(y), x);
  }

  /**
   * Math.atan2() for Variables.
   *
   * @param y The y argument.
   * @param x The x argument.
   * @return Result of atan2().
   */
  public static Variable atan2(Variable y, double x) {
    return atan2(y, new Variable(x));
  }

  /**
   * Math.atan2() for Variables.
   *
   * @param y The y argument.
   * @param x The x argument.
   * @return Result of atan2().
   */
  public static Variable atan2(Variable y, Variable x) {
    return fromHandle(VariableJNI.atan2(y.getHandle(), x.getHandle()));
  }

  /**
   * Math.cbrt() for Variables.
   *
   * @param x The argument.
   * @return Result of cbrt().
   */
  public static Variable cbrt(Variable x) {
    return fromHandle(VariableJNI.cbrt(x.getHandle()));
  }

  /**
   * Math.cos() for Variables.
   *
   * @param x The argument.
   * @return Result of cos().
   */
  public static Variable cos(Variable x) {
    return fromHandle(VariableJNI.cos(x.getHandle()));
  }

  /**
   * Math.cosh() for Variables.
   *
   * @param x The argument.
   * @return Result of cosh().
   */
  public static Variable cosh(Variable x) {
    return fromHandle(VariableJNI.cosh(x.getHandle()));
  }

  /**
   * Math.exp() for Variables.
   *
   * @param x The argument.
   * @return Result of exp().
   */
  public static Variable exp(Variable x) {
    return fromHandle(VariableJNI.exp(x.getHandle()));
  }

  /**
   * Math.hypot() for Variables.
   *
   * @param x The x argument.
   * @param y The y argument.
   * @return Result of hypot().
   */
  public static Variable hypot(double x, Variable y) {
    return hypot(new Variable(x), y);
  }

  /**
   * Math.hypot() for Variables.
   *
   * @param x The x argument.
   * @param y The y argument.
   * @return Result of hypot().
   */
  public static Variable hypot(Variable x, double y) {
    return hypot(x, new Variable(y));
  }

  /**
   * Math.hypot() for Variables.
   *
   * @param x The x argument.
   * @param y The y argument.
   * @return Result of hypot().
   */
  public static Variable hypot(Variable x, Variable y) {
    return fromHandle(VariableJNI.hypot(x.getHandle(), y.getHandle()));
  }

  /**
   * Math.hypot() for Variables.
   *
   * @param x The x argument.
   * @param y The y argument.
   * @param z The z argument.
   * @return Result of hypot().
   */
  public static Variable hypot(double x, double y, Variable z) {
    return hypot(new Variable(x), new Variable(y), z);
  }

  /**
   * Math.hypot() for Variables.
   *
   * @param x The x argument.
   * @param y The y argument.
   * @param z The z argument.
   * @return Result of hypot().
   */
  public static Variable hypot(double x, Variable y, double z) {
    return hypot(new Variable(x), y, new Variable(z));
  }

  /**
   * Math.hypot() for Variables.
   *
   * @param x The x argument.
   * @param y The y argument.
   * @param z The z argument.
   * @return Result of hypot().
   */
  public static Variable hypot(double x, Variable y, Variable z) {
    return hypot(new Variable(x), y, z);
  }

  /**
   * Math.hypot() for Variables.
   *
   * @param x The x argument.
   * @param y The y argument.
   * @param z The z argument.
   * @return Result of hypot().
   */
  public static Variable hypot(Variable x, double y, double z) {
    return hypot(x, new Variable(y), new Variable(z));
  }

  /**
   * Math.hypot() for Variables.
   *
   * @param x The x argument.
   * @param y The y argument.
   * @param z The z argument.
   * @return Result of hypot().
   */
  public static Variable hypot(Variable x, double y, Variable z) {
    return hypot(x, new Variable(y), z);
  }

  /**
   * Math.hypot() for Variables.
   *
   * @param x The x argument.
   * @param y The y argument.
   * @param z The z argument.
   * @return Result of hypot().
   */
  public static Variable hypot(Variable x, Variable y, double z) {
    return hypot(x, y, new Variable(z));
  }

  /**
   * Math.hypot() for Variables.
   *
   * @param x The x argument.
   * @param y The y argument.
   * @param z The z argument.
   * @return Result of hypot().
   */
  public static Variable hypot(Variable x, Variable y, Variable z) {
    return sqrt(pow(x, 2).plus(pow(y, 2)).plus(pow(z, 2)));
  }

  /**
   * Math.pow() for Variables.
   *
   * @param base The base.
   * @param power The power.
   * @return Result of pow().
   */
  public static Variable pow(double base, Variable power) {
    return pow(new Variable(base), power);
  }

  /**
   * Math.pow() for Variables.
   *
   * @param base The base.
   * @param power The power.
   * @return Result of pow().
   */
  public static Variable pow(Variable base, double power) {
    return pow(base, new Variable(power));
  }

  /**
   * Math.pow() for Variables.
   *
   * @param base The base.
   * @param power The power.
   * @return Result of pow().
   */
  public static Variable pow(Variable base, Variable power) {
    return fromHandle(VariableJNI.pow(base.getHandle(), power.getHandle()));
  }

  /**
   * Math.log() for Variables.
   *
   * @param x The argument.
   * @return Result of log().
   */
  public static Variable log(Variable x) {
    return fromHandle(VariableJNI.log(x.getHandle()));
  }

  /**
   * Math.log10() for Variables.
   *
   * @param x The argument.
   * @return Result of log10().
   */
  public static Variable log10(Variable x) {
    return fromHandle(VariableJNI.log10(x.getHandle()));
  }

  /**
   * sign() for Variables.
   *
   * @param x The argument.
   * @return Result of sign().
   */
  public static Variable sign(Variable x) {
    return fromHandle(VariableJNI.sign(x.getHandle()));
  }

  /**
   * Math.sin() for Variables.
   *
   * @param x The argument.
   * @return Result of sin().
   */
  public static Variable sin(Variable x) {
    return fromHandle(VariableJNI.sin(x.getHandle()));
  }

  /**
   * Math.sinh() for Variables.
   *
   * @param x The argument.
   * @return Result of sinh().
   */
  public static Variable sinh(Variable x) {
    return fromHandle(VariableJNI.sinh(x.getHandle()));
  }

  /**
   * Math.sqrt() for Variables.
   *
   * @param x The argument.
   * @return Result of sqrt().
   */
  public static Variable sqrt(Variable x) {
    return fromHandle(VariableJNI.sqrt(x.getHandle()));
  }

  /**
   * Math.tan() for Variables.
   *
   * @param x The argument.
   * @return Result of tan().
   */
  public static Variable tan(Variable x) {
    return fromHandle(VariableJNI.tan(x.getHandle()));
  }

  /**
   * Math.tanh() for Variables.
   *
   * @param x The argument.
   * @return Result of tanh().
   */
  public static Variable tanh(Variable x) {
    return fromHandle(VariableJNI.tanh(x.getHandle()));
  }
}
